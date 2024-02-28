#include "consts.h"

char *OUTPUT_DIR = "/vol8/home/hnu_xiaolab_5/mimic/output/";
#ifdef MIMICIII
char *data_path = "/vol8/home/hnu_xiaolab_5/mimic/data/mimiciii/";
#elif defined MIMICIV
char *data_path = "/vol8/home/hnu_xiaolab_5/mimic/data/mimiciv/";
#endif

unsigned int hash_table_len = 0;

#ifdef MIMICIII
const int ID_LEN = 651048;
const int TRANSFERS_LEN = 261897;
const int PATIENTS_LEN = 46520;
const int ADMISSION_LEN = 58976;
const int NOTEEVENT_LEN = 1851344;
const int PRESCRIPTION_LEN = 4156449;
const int ICUSTAY_LEN = 0;
#elif defined(MIMICIV)
const int ID_LEN = 5280352;
const int ICUSTAY_LEN = 76540;
const int TRANSFERS_LEN = 2189536;
const int ADMISSION_LEN = 523741;
const int PATIENTS_LEN = 382279;
const int PRESCRIPTION_LEN = 17008053;
const int LABEVENTS_LEN = 65038819;
const int NOTEEVENT_LEN = 0;
#endif

char DIAGNOSES_ICD[512];
char PRESCRIPTION[512];
char NOTEEVENT[512];
char PATIENTS[512];
char ADMISSIONS[512];
char LABEVENTS[512];
char CHARTEVENTS[512];
char OUTPUTEVENTS[512];
char ICUSTAYS[512];
char TRANSFERS[512];

const int CSV_STR_MAX_LEN = 100 * 1024 * 1024;