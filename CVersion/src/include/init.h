#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "defines.h"

extern char *OUTPUT_DIR;
extern char *data_path;
extern unsigned int hash_table_len;

extern char DIAGNOSES_ICD[512];
extern char PRESCRIPTION[512];
extern char NOTEEVENT[512];
extern char PATIENTS[512];
extern char ADMISSIONS[512];
extern char LABEVENTS[512];
extern char CHARTEVENTS[512];
extern char OUTPUTEVENTS[512];
#ifdef MIMICIV
extern char ICUSTAYS[512];
#elif defined(MIMICIII)
extern char TRANSFERS[512];
#endif

int init(int mpi_size);