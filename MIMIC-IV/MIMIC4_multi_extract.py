import pymssql
import pandas as pd
import numpy as np
import csv
from multiprocessing import Pool
import time,multiprocessing,re

import dill as pickle

"""配置"""
server='127.0.0.1' #服务器IP地址
user ='sa' #用户名
password='123456' #密码
database='mimic4' #数据库名称
# port='1433'

"""连接数据库"""
conn = pymssql.connect(server, user, password, database)
cursor = conn.cursor()

def get_connect():
	conn1 = pymssql.connect(server, user, password, database)
	return conn1

H_ID = [] #raw
hadm = [] #改格式后

cursor.execute(r"SELECT  [hadm_id] FROM [mimic4].[dbo].[MIMIC4_diagnoses_icd]")
H_ID = cursor.fetchall()
#数据格式处理
for HADMid in (H_ID):
	HADMid = re.findall('\d+', str(HADMid))
	HADMid = re.sub(r'[^\d]', ' ', str(HADMid))
	HADMid = int(HADMid)
	hadm.append(HADMid)
# print("HADM:",hadm)
print("len_HADM",len(hadm))

#列表去重
ids = list(set(hadm))
print("set hadm",len(ids))


def get_SUBJECT_ID(HADM_ID): #通过HADM_ID查询Sub_id
    subjectid = []
    cursor.execute(r"exec MIMIC4_check_subject @hadm_id="+str(HADM_ID))
    sub_ID = cursor.fetchall()  # 查询得到sub id
    for sub_id in (sub_ID): #数据格式处理
        sub_id = re.findall('\d+', str(sub_id))
        sub_id = re.sub(r'[^\d]', ' ', str(sub_id))
        sub_id= int(sub_id)
        # print("sub_id",sub_id)
        subjectid.append(sub_id)
    return subjectid


ALL_SUB = [] #存所有的Sub_id
for i in ids:
	id = get_SUBJECT_ID(i)  #获取Sub_id
	ALL_SUB.append(id) #逐个存入列表

# sub_end=time.time()
# print('sub time cost',sub_end - sub_start,'s')

# print("SUB",ALL_SUB)
print("len_SUB",len(ALL_SUB ))
# print()

# ALL_SUB = [58526], [54610], [9895], [23018], [533], [55853], [87977], [60039], [23804], [68591]
# ids = [100001, 100003, 100006, 100007, 100009, 100010, 100011, 100012, 100014, 100016]

# ALL_SUB = [10600653]
# ids = [22409835]

def create_dataset(data):
    # conn1 = get_connect()
    SUBJECT_ID, HADM_ID = data
    # print("输入",SUBJECT_ID, HADM_ID)

    feature = ['HR',	'O2Sat',	'Temp',	'SBP',	'MAP',	'DBP',	'Resp',	'EtCO2',	'BaseExcess',	'HCO3',	'FiO2',	'pH',
                    'PaCO2',	'SaO2',	'AST',	'BUN',
    'Alkalinephos',	'Calcium',	'Chloride',	'Creatinine',	'Bilirubin_direct',	'Glucose',	'Lactate',	'Magnesium',
                    'Phosphate',	'Potassium',	'Bilirubin_total',
    'TroponinI',	'Hct',	'Hgb',	'PTT',	'WBC',	'Fibrinogen',	'Platelets','Age',	'Gender',	'Unit1',	'Unit2',
               'HospAdmTime',	'ICULOS','Strategy','GSN','NDC','DRUG']

    T1 = time.time()
    cursor.execute(r"exec p_data_be_MIMIC4 @subject_id="+str(SUBJECT_ID)+",@hadm_id="+str(HADM_ID))
    Result = cursor.fetchall()  #得到结果集
    TS_Result = pd.DataFrame(Result)
    #获取开始结束时间
    if TS_Result.empty == False:
        Btime = TS_Result.iloc[0,-2]
        Etime = TS_Result.iloc[-1,-1]

        TS_Result['Strategy'] = None
        TS_Result['GSN'] = None
        TS_Result['NDC'] = None
        TS_Result['DRUG'] = None


        T2 = time.time()


        #获取药物数据
        cursor.execute(r"exec MIMIC4_drug_diff @hadm_id="+str(HADM_ID))
        drug_list = cursor.fetchall()
        drug = pd.DataFrame(drug_list)
        print('TS data time(ms)' , ((T2 - T1)*1000))
        #病例药物信息逐个填入
        for o in range(len(drug_list)):
            stime = drug.iloc[o,0] #开始时间
            diff = drug.iloc[o,-1] #药物生效时间
            Strategy = drug.iloc[o,2] #用药策略
            GSN = drug.iloc[o,3] #GSN编码
            NDC = drug.iloc[o,4] #NDC编码
            DRUG = drug.iloc[o,5] #药物名称
            #逐个检查时序表
            for len_Result in range(len(Result)):
                if str(stime) == str(TS_Result.iloc[len_Result,40]): #该药物第一次用药匹配

                    for ass_value in range(int(diff)):#
                        # 判断单元格是否为空，如果为空
                        if TS_Result.iloc[len_Result+int(ass_value),-1] is None:
                            TS_Result.iloc[len_Result + int(ass_value), -4] = [Strategy] #是直接填入
                            TS_Result.iloc[len_Result + int(ass_value), -3] = [GSN] #是直接填入
                            TS_Result.iloc[len_Result + int(ass_value), -2] = [NDC] #是直接填入
                            TS_Result.iloc[len_Result + int(ass_value), -1] = [DRUG]  # 是直接填入

                        # 否则保留当前值，在当前值后填入
                        if TS_Result.iloc[len_Result+int(ass_value),-1] is not None:
                            raw_Strategy = str(TS_Result.iloc[len_Result+int(ass_value),-4])
                            raw_GSN = str(TS_Result.iloc[len_Result + int(ass_value), -3])
                            raw_NDC = str(TS_Result.iloc[len_Result + int(ass_value), -2])
                            raw_DRUG = str(TS_Result.iloc[len_Result + int(ass_value), -1])

                            TS_Result.iloc[len_Result+int(ass_value),-4] = [str(raw_Strategy)+","+str(Strategy)]
                            TS_Result.iloc[len_Result + int(ass_value), -3] = [str(raw_GSN)+","+str(GSN)]
                            TS_Result.iloc[len_Result + int(ass_value), -2] = [str(raw_NDC)+","+str(NDC)]
                            TS_Result.iloc[len_Result + int(ass_value), -1] = [str(raw_DRUG) + "," + str(DRUG)]

                            raw_Strategy = None #RAW 清空
                            raw_GSN = None  # RAW 清空
                            raw_NDC = None  # RAW 清空
                            raw_DRUG= None  # RAW 清空
                    break
        T3 = time.time()
        print('Drug time(ms)' , ((T3 - T2)*1000))
        del TS_Result[41]
        del TS_Result[40]



        output_path = "C://Users//pdl//Desktop//MIMIC4_csv//"
        with open(output_path+str(HADM_ID)+".csv", "w",newline='') as f:
            writer = csv.writer(f)
            writer.writerow(feature) #写入表头
            #格式转换
            final = np.array(TS_Result)
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

    from multiprocessing import Pool
    import time, multiprocessing, re


    pool = Pool(multiprocessing.cpu_count())

    pool.map(create_dataset, zip(ALL_SUB, ids))
    pool.close()

    csv_end=time.time()
    print('final time(ms)' , ((csv_end - csv_start)*1000))