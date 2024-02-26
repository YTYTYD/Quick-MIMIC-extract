from extsdk.icdCodeConvert import get_icd_code
import struct


class ext_task:
    def __init__(self):
        self.age = 1
        self.gender = 1
        self.Unit1 = 1
        self.Unit2 = 1
        self.HospAdmTime = 1
        self.ICULOS = 1
        self.drug = 1
        self.note = 1
        self.task_id = 123
        # 提取开始时间, 为0表示从第一条记录开始, 为1表示从入院时间开始, 2表示从二者最小值中选择
        self.start_time = 0
        self.ICD_list = []
        self.feature_ids = {}
        self.is_MIMIC_IV = False

    def pack(self):
        data = struct.pack('iiiiiiiiiiii', len(self.feature_ids),
                           len(self.ICD_list),
                           self.age,
                           self.gender,
                           self.Unit1,
                           self.Unit2,
                           self.HospAdmTime,
                           self.ICULOS,
                           self.drug,
                           self.note,
                           self.task_id,
                           self.start_time)
        icd_list = b''
        for icd_code in self.ICD_list:
            if type(icd_code) != str:
                icd_code = str(icd_code)
            icd_list += struct.pack('i', get_icd_code(icd_code, self.is_MIMIC_IV))

        features = b''
        for (name, item_ids) in self.feature_ids.items():
            features += struct.pack('i', len(item_ids))
            features += name.encode("utf-8") + b'\x00'
            for item_id in item_ids:
                features += struct.pack('i', item_id)
        return data + icd_list + features
