import pymssql
import pandas as pd
import numpy as np

from multiprocessing import Pool
import time,multiprocessing,re

import dill as pickle

"""配置"""
server='127.0.0.1'
user ='sa'
password='123456'
database='mimic'
# port='1433'
"""连接数据库"""

conn = pymssql.connect(server, user, password, database)
cursor = conn.cursor()

def get_connect():
	conn1 = pymssql.connect(server, user, password, database)
	return conn1

H_ID = [] #原始raw
hadm = [] #改格式后
cursor.execute(r"SELECT  [HADM_ID] FROM [mimic].[dbo].[DIAGNOSES_ICD]")
H_ID = cursor.fetchall()
#数据格式处理
for HADMid in (H_ID):
	HADMid = re.findall('\d+', str(HADMid))
	HADMid = re.sub(r'[^\d]', ' ', str(HADMid))
	HADMid = int(HADMid)
	# print("HADMid",HADMid)
	hadm.append(HADMid)
print("HADM:",hadm)
print("len_HADM",len(hadm))
#mimic中5325脓毒症病例,4689名患者
#去重
ids = list(set(hadm))
print("set hadm",ids)


def get_SUBJECT_ID(HADM_ID):
    subjectid = []
    cursor.execute(r"exec check_subject @HADM_ID="+str(HADM_ID))
    sub_ID = cursor.fetchall()  # 得到id
    for sub_id in (sub_ID):
        sub_id = re.findall('\d+', str(sub_id))
        sub_id = re.sub(r'[^\d]', ' ', str(sub_id))
        sub_id= int(sub_id)
        # print("sub_id",sub_id)
        subjectid.append(sub_id)
    return subjectid

sub_start=time.time()
ALL_SUB = [] #存所有的SUBJECT_ID
for i in ids:
	id = get_SUBJECT_ID(i)  #
	ALL_SUB.append(id)
	#
sub_end=time.time()
print('sub time cost',sub_end - sub_start,'s')

print("SUB",ALL_SUB)
print("len_SUB",len(ALL_SUB ))
print()
# ALL_SUB = [58526], [54610], [9895], [23018], [533], [55853], [87977], [60039], [23804], [68591]
# ids = [100001, 100003, 100006, 100007, 100009, 100010, 100011, 100012, 100014, 100016]

def create_dataset(data):
    # conn1 = get_connect()
    SUBJECT_ID, HADM_ID = data
    # print("输入",SUBJECT_ID, HADM_ID)

    feature = ['HR',	'O2Sat',	'Temp',	'SBP',	'MAP',	'DBP',	'Resp',	'EtCO2',	'BaseExcess',	'HCO3',	'FiO2',	'pH',
                    'PaCO2',	'SaO2',	'AST',	'BUN',
    'Alkalinephos',	'Calcium',	'Chloride',	'Creatinine',	'Bilirubin_direct',	'Glucose',	'Lactate',	'Magnesium',
                    'Phosphate',	'Potassium',	'Bilirubin_total',
    'TroponinI',	'Hct',	'Hgb',	'PTT',	'WBC',	'Fibrinogen',	'Platelets','Age',	'Gender',	'Unit1',	'Unit2',
               'HospAdmTime',	'ICULOS','Strategy','GSN','NDC','TEXT']

    T1 = time.time()
    cursor.execute(r"exec p_data_be_test @SUBJECT_ID="+str(SUBJECT_ID)+",@HADM_ID="+str(HADM_ID))
    Result = cursor.fetchall()  #得到结果集
    TS_Result = pd.DataFrame(Result)
    #获取开始结束时间
    if TS_Result.empty == False:
        Btime = TS_Result.iloc[0,-2]
        Etime = TS_Result.iloc[-1,-1]

        TS_Result['Strategy'] = None
        TS_Result['GSN'] = None
        TS_Result['NDC'] = None


        T2 = time.time()


        #药物数据
        cursor.execute(r"exec drug_diff @HADM_ID="+str(HADM_ID))
        drug_list = cursor.fetchall()
        drug = pd.DataFrame(drug_list)
        print('TS data time(ms)' , ((T2 - T1)*1000))

        #药物列表循环
        for o in range(len(drug_list)):
            stime = drug.iloc[o,0]
            diff = drug.iloc[o,-1]
            strategy = drug.iloc[o,2]
            GSN = drug.iloc[o,3]
            NDC = drug.iloc[o,4]
            #逐个检查时序表
            for len_Result in range(len(Result)):
                if str(stime) == str(TS_Result.iloc[len_Result,40]): #该药物第一次时间匹配
                    # 在原有基础上赋值

                    for ass_value in range(int(diff)):
                        # 判断单元格是否为空
                        if TS_Result.iloc[len_Result+int(ass_value),-1] is None:
                            TS_Result.iloc[len_Result + int(ass_value), -3] = [strategy] #是直接填入
                            TS_Result.iloc[len_Result + int(ass_value), -2] = [GSN] #是直接填入
                            TS_Result.iloc[len_Result + int(ass_value), -1] = [NDC] #是直接填入

                        # 否则保留当前值后填入
                        if TS_Result.iloc[len_Result+int(ass_value),-1] is not None:
                            raw_strategy = str(TS_Result.iloc[len_Result+int(ass_value),-3])
                            raw_GSN = str(TS_Result.iloc[len_Result + int(ass_value), -2])
                            raw_NDC = str(TS_Result.iloc[len_Result + int(ass_value), -1])

                            TS_Result.iloc[len_Result+int(ass_value),-3] = [str(raw_strategy)+","+str(strategy)]
                            TS_Result.iloc[len_Result + int(ass_value), -2] = [str(raw_GSN)+","+str(GSN)]
                            TS_Result.iloc[len_Result + int(ass_value), -1] = [str(raw_NDC)+","+str(NDC)]

                            raw_strategy = None #RAW 清空
                            raw_GSN = None  # RAW 清空
                            raw_NDC = None  # RAW 清空
                    break
        T3 = time.time()
        print('Drug time(ms)' , ((T3 - T2)*1000))

        # T3 = time.time()
        cursor.execute(r"exec extract1 @HADM_ID="+str(HADM_ID)+",@begintime ='"+str(Btime)+ "',@endtime = '"+str(Etime)+"'")
        text_result = cursor.fetchall()  #得到结果集
        text = pd.DataFrame(text_result)

        final = pd.concat([TS_Result,text],axis= 1)

        del final[41]
        del final[40]


        T4 = time.time()
        print('text time(ms)' , ((T4 - T3)*1000))


        import csv
        with open(r'C://Users//pdl//Desktop//csv//'+str(HADM_ID)+".csv", "w",newline='') as f:
            writer = csv.writer(f)
            writer.writerow(feature) #写入表头
            #格式转换
            final = np.array(final)
            final = final.tolist()
            #去空值
            for result in final:
                result = list(result)
                for z in range(len(result)):
                    result[z] = str(result[z])
                    if "nan" in result[z]:
                        result[z] = ""
                    if "None" in result[z]:
                        result[z] = ""
                writer.writerow(result)  # 遍历输出csv
    # conn1.close()
        T5 = time.time()
        print('total time(ms)' , ((T5 - T1)*1000))
        print("--------------------------------")

    return




if __name__ == '__main__':
    csv_start = time.time()

    # pool = Pool(processes=1)
    #并行
    pool = Pool(multiprocessing.cpu_count())

    pool.map(create_dataset, zip(ALL_SUB, ids))
    pool.close()

    csv_end=time.time()
    print('final time(ms)' , ((csv_end - csv_start)*1000))


