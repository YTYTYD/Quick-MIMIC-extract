import extsdk

ip = "192.168.1.1"
port = 9812

s = extsdk.extracter()
s.connect(ip, port)

# config
s.task.note = 0
s.task.drug = 0
s.task.start_time = 0
# set ICD code list
s.task.ICD_list = ["99984", "99989", "9999",
                   "E0000", "E0008", "E0009", "E0010", "E0011"]
s.task.is_MIMIC_IV = False
# set item ids
s.task.feature_ids["HR"] = [211, 220045]
s.task.feature_ids["O2Sat"] = [50817, 220277]
s.task.feature_ids["Temp"] = [223762, 678, 679]

# start extraction
ids, d = s.start_extract(raw=True)
print(len(ids))
print(ids)
# ids: HADM_ID
# d: extraction result
