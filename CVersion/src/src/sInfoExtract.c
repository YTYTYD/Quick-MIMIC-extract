#include "sInfoExtract.h"

/**
 * 读取PATIENTS表中的出生日期信息与性别信息
 */
void PATIENTS_read(int SUBJECT_ID, int *gender, int *age, int ADMIYEAR)
{
    struct PATIENTS_DATA *temp;
    if (find_PATIENTS_NODE(PATIENTS_TABLE, SUBJECT_ID, &temp) == -1)
        return;
    *gender = temp->GENDER;
#ifdef MIMICIII
    *age = ADMIYEAR - temp->YOB;
#elif defined MIMICIV
    *age = temp->AGE;
#endif
}

/**
 * 读取ADMISSIONS表中的入院时间信息
 */
void ADMISSIONS_read(int HADM_ID, long long *ADMITIME, int *ADMIYEAR)
{
    struct ADMISSIONS_DATA *temp;
    if (find_ADMISSIONS_NODE(ADMISSIONS_TABLE, HADM_ID, &temp) == -1)
        return;
    *ADMIYEAR = temp->ADMIYEAR;
    *ADMITIME = temp->ADMITIME;
}

void sInfo_extract(struct StaticInformation *sInfo, int HID, int SUBJECT_ID)
{
    // 读取入院时间等信息
    long long ADMITIME; // 入院时间
    int ADMIYEAR;       // 入院年份
    ADMISSIONS_read(HID, &ADMITIME, &ADMIYEAR);
    sInfo->HospAdmiTime = ADMITIME; // 住院时长由之后的endtime计算, 此处只返回入院时间

    PATIENTS_read(SUBJECT_ID, &sInfo->Gender, &sInfo->Age, ADMIYEAR);
    // 读取是否在MICU或SICU
    sInfo->Unit1 = 0;   //MICU
    sInfo->Unit2 = 0;   //SICU
#ifdef MIMICIII
    find_TRANSFER_NODE(TRANSFERS_TABLE, HID, &sInfo->Unit1, &sInfo->Unit2);
#elif defined MIMICIV
    find_ICUSTAY_NODE(ICUSTAY_TABLE, HID, &sInfo->Unit1, &sInfo->Unit2);
#endif
}