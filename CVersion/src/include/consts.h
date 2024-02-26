#pragma once

#include "defines.h"

extern char *OUTPUT_DIR;
extern char *data_path;

extern unsigned int hash_table_len;

extern const int ID_LEN;
extern const int TRANSFERS_LEN;
extern const int PATIENTS_LEN;
extern const int ADMISSION_LEN;
extern const int NOTEEVENT_LEN;
extern const int PRESCRIPTION_LEN;
extern const int ICUSTAY_LEN;
extern const int LABEVENTS_LEN;

extern char DIAGNOSES_ICD[512];
extern char PRESCRIPTION[512];
extern char NOTEEVENT[512];
extern char PATIENTS[512];
extern char ADMISSIONS[512];
extern char LABEVENTS[512];
extern char CHARTEVENTS[512];
extern char OUTPUTEVENTS[512];
extern char ICUSTAYS[512];
extern char TRANSFERS[512];

extern const int CSV_STR_MAX_LEN;