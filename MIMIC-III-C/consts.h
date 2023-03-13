// #define MIMICIV
#define MIMICIII

// 要提取的特征列表与输出路径
char FEATURES_LIST[] = "D:/MIMIC/3/featureslist.txt";
char OUTPUT_DIR[] = "D:/MIMIC/3/outputtest/";

char data_path[] = "F:/MIMIC/mimic-iv-1.0/data/";
char index_path[] = "F:/MIMIC/mimiciv index/";

#ifdef MIMICIII
const int PRESCRIPTION_LEN = 4156449;
const int NOTEEVENT_LEN = 1851344;
const int TRANSFERS_LEN = 261897;
const int LABEVENTS_LEN = 22245034;
#elif defined(MIMICIV)
const int PRESCRIPTION_LEN = 17008053;
const int LABEVENTS_LEN = 65038819;
#endif

char PRESCRIPTION_INDEX[512];
char NOTEEVENT_INDEX[512];
char LABEVENTS_INDEX[512];
char CHARTEVENTS_INDEX[512];

char DIAGNOSES_ICD[512];
char PRESCRIPTION[512];
char NOTEEVENT[512];
char PATIENTS[512];
char ADMISSIONS[512];
char LABEVENTS[512];
char CHARTEVENTS[512];
#ifdef MIMICIV
char ICUSTAYS[512];
#elif defined(MIMICIII)
char TRANSFERS[512];
#endif
