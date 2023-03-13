/**
 * 读取PATIENTS表中的出生日期信息与性别信息
 */
void PATIENTS_read(int SUBJECT_ID, int *gender, int *age)
{
    struct PATIENTS_DATA *temp;
    if (find_PATIENTS_NODE(PATIENTS_TABLE, SUBJECT_ID, &temp) == -1)
        return;
    *gender = temp->GENDER;
    *age = temp->AGE;
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
    // 读取是否在MICU或SICU
    sInfo->Util1 = 0;
    sInfo->Util2 = 0;
    find_ICUSTAY_NODE(ICUSTAY_TABLE, HID, &sInfo->Util1, &sInfo->Util2);
    PATIENTS_read(SUBJECT_ID, &sInfo->Gender, &sInfo->Age);
}