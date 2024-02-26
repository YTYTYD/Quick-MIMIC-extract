#pragma once
#include "defines.h"
#include "patientsPRead.h"
#include "admissionsPRead.h"
#include "transfersPRead.h"
#include "icustaysExtractPRead.h"

extern struct PATIENTS_DATA *PATIENTS_TABLE;
extern struct ADMISSIONS_DATA *ADMISSIONS_TABLE;
extern struct TRANSFER_DATA *TRANSFERS_TABLE;

// 病人基本信息
struct StaticInformation
{
    int Age;
    int Gender; // 性别, 男为1, 女为0
    int Unit1;
    int Unit2;
    long long HospAdmiTime;
    long long begintime;
    long long endtime;
};

void PATIENTS_read(int SUBJECT_ID, int *gender, int *age, int ADMIYEAR);
void ADMISSIONS_read(int HADM_ID, long long *ADMITIME, int *ADMIYEAR);
void sInfo_extract(struct StaticInformation *sInfo, int HID, int SUBJECT_ID);