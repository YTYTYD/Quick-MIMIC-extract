/**
 * 读取PATIENTS表中的出生日期信息与性别信息
 */
void PATIENTS_read(FILE *index_file, int SUBJECT_ID, FILE *csv_file, int *gender, int *age, int ADMIYEAR)
{
    // int gender;   // 性别, 1为男性, 0为女性
    char DOB[64]; // 出生日期

    // 获取索引
    unsigned int *offset = (unsigned int *)malloc(10 * sizeof(unsigned int)); // 开了10个大小的数组, 实际应该只有一个
    unsigned int offset_size = 0;
    offset_size = get_all_offset(SUBJECT_ID, index_file, offset, 46520);

#ifdef DEBUG
    if (offset_size > 1)
    {
        printf("ERROR! code:764837465\n");
    }
#endif

    // 根据索引读取文件
    // 一个SUBJECT_ID应该只对应一条信息, 这里直接读取第一条
    char buffer[1024];
    fseek(csv_file, offset[0], SEEK_SET);
    fgets(buffer, 1024, csv_file);

    unsigned int index = 0;

    // 读取性别信息
    buff_index_move(buffer, &index, 2);
    index += 1;
    if (fgetc(csv_file) == 'F')
        *gender = 1;
    else
        *gender = 0;

    // 读取出生日期
    buff_index_move(buffer, &index, 1);
    str_cpy(buffer, DOB, &index, ',');
    int yob = my_atoi(DOB);
    *age = ADMIYEAR - yob;

    free(offset);
}

/**
 * 读取TRANSFERS表
 */
void TRANSFERS_read(FILE *index_file, int HADM_ID, FILE *csv_file, int *MICU, int *SICU)
{
    // int MICU = 0; // 是否在MICU
    // int SICU = 0; // 是否在SICU

    const int MAX_SIZE = 1024;
    // 偏移量获取
    unsigned int *offset = (unsigned int *)malloc(MAX_SIZE * sizeof(unsigned int));
    unsigned int offset_size = 0;
    offset_size = get_all_offset(HADM_ID, index_file, offset, 261897);

    // 开始读取
    char buffer[1024];
    char CURR_CAREUNIT[32];
    int i = 0;
    for (i = 0; i < offset_size; i++)
    {
        unsigned int index = 0;
        fseek(csv_file, offset[i], SEEK_SET);
        fgets(buffer, 1024, csv_file);
        buff_index_move(buffer, &index, 7);
        str_cpy(buffer, CURR_CAREUNIT, &index, ',');
        if (strcmp(CURR_CAREUNIT, "MICU") == 0)
            *MICU = 1;
        else if (strcmp(CURR_CAREUNIT, "SICU") == 0)
            *SICU = 1;
    }

    //
    free(offset);
}

/**
 * 读取ADMISSIONS表中的入院时间信息
 * TODO:之后要根据入院时间计算住院时间
 */
void ADMISSIONS_read(FILE *index_file, int HADM_ID, FILE *csv_file, long long *ADMITIME, int *ADMIYEAR, long long *DISCHTIME)
{
    char ADMITIME_C[64];
    char DISCHTIME_C[64];

    // 偏移量获取
    unsigned int *offset = (unsigned int *)malloc(10 * sizeof(unsigned int)); // 开了10个大小的数组, 实际应该只有一个
    unsigned int offset_size = 0;
    offset_size = get_all_offset(HADM_ID, index_file, offset, 58976);

#ifdef DEBUG
    // 理论上应该只存在一个
    if (offset_size > 1)
        printf("ERROD! code:723493274\n"); // 随机的一串数字, 没啥意思, 方便定位而已
#endif
    char buffer[1024];
    unsigned int index = 0;
    fseek(csv_file, offset[0], SEEK_SET);
    fgets(buffer, 1024, csv_file);
    buff_index_move(buffer, &index, 3);
    str_cpy(buffer, ADMITIME_C, &index, ',');
    buff_index_move(buffer, &index, 1);
    str_cpy(buffer, DISCHTIME_C, &index, ',');
    *ADMIYEAR = my_atoi(ADMITIME_C);
    *ADMITIME = str_2_time_stamp(ADMITIME_C);
    *DISCHTIME = str_2_time_stamp(DISCHTIME_C);

    free(offset);
}

void sInfo_extract(struct StaticInformation *sInfo, int HID, int SUBJECT_ID)
{
    FILE *ADMISSIONS_csv = fopen(ADMISSIONS, "r");
    FILE *TRANSFERS_csv = fopen(TRANSFERS, "r");
    FILE *PATIENTS_csv = fopen(PATIENTS, "r");
    FILE *ADMISSIONS_index = fopen(ADMISSIONS_INDEX, "rb");
    FILE *TRANSFERS_index = fopen(TRANSFERS_INDEX, "rb");
    FILE *PATIENTS_index = fopen(PATIENTS_INDEX, "rb");

    // 读取入院时间等信息
    long long ADMITIME;  // 入院时间
    long long DISCHTIME; // 出院时间
    int ADMIYEAR;        // 入院年份
    ADMISSIONS_read(ADMISSIONS_index, HID, ADMISSIONS_csv, &ADMITIME, &ADMIYEAR, &DISCHTIME);
    // sInfo->HospAdmiTime = (DISCHTIME - ADMITIME) / 3600;    //计算住院时长(小时)
    sInfo->HospAdmiTime = ADMITIME; // 住院时长由之后的endtime计算, 此处只返回入院时间
    // 读取是否在MICU或SICU
    sInfo->Util1 = 0;
    sInfo->Util2 = 0;
    TRANSFERS_read(TRANSFERS_index, HID, TRANSFERS_csv, &sInfo->Util1, &sInfo->Util2);
    PATIENTS_read(PATIENTS_index, SUBJECT_ID, PATIENTS_csv, &sInfo->Gender, &sInfo->Age, ADMIYEAR);

    fclose(ADMISSIONS_csv);
    fclose(TRANSFERS_csv);
    fclose(PATIENTS_csv);
    fclose(PATIENTS_index);
    fclose(TRANSFERS_index);
    fclose(ADMISSIONS_index);
    // printf("%lld %d %d %d %d %d\n", ADMITIME, sInfo->Util1, sInfo->Util2, sInfo->Gender, sInfo->Age, sInfo->HospAdmiTime);
}