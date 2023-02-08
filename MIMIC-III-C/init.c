/**
 * 检查文件是否存在
 */
int file_check(char *path)
{
    int r = 0;
    FILE *f = fopen(path, "r");
    if (f != NULL)
    {
        r = 1;
        fclose(f);
    }
    return r;
}

/**
 * 拼接两个字符串到dst并检查路径是否存在
 */
int path_merge(char *dst, char *path, const char *file_name)
{
    strcpy(dst, path);
    strcpy(dst + strlen(path), file_name);
    if (file_check(dst) == 0)
    {
        printf("%s 不存在\n", file_name);
        return 0;
    }
    return 1;
}

int init()
{
    int r = 0;
    if(access(OUTPUT_DIR,0)!=0){
        printf("输出路径不存在\n");
        r = -1;
    }
    if(file_check(FEATURES_LIST) != 1){
        printf("特征列表文件不存在\n");
        r = -1;
    }
    //依次检查每个文件是否存在
    if (!path_merge(DIAGNOSES_ICD, data_path, "DIAGNOSES_ICD.csv")
    || !path_merge(PRESCRIPTION, data_path, "PRESCRIPTIONS.csv")
    || !path_merge(NOTEEVENT, data_path, "NOTEEVENTS.csv")
    || !path_merge(PATIENTS, data_path, "PATIENTS.csv")
    || !path_merge(ADMISSIONS, data_path, "ADMISSIONS.csv")
    || !path_merge(TRANSFERS, data_path, "TRANSFERS.csv")
    || !path_merge(LABEVENTS, data_path, "LABEVENTS.csv")
    || !path_merge(CHARTEVENTS, data_path, "CHARTEVENTS.csv")
    || !path_merge(PRESCRIPTION_INDEX, index_path, "PRESCRIPTIONS_INDEX")
    || !path_merge(NOTEEVENT_INDEX, index_path, "NOTEEVENT_INDEX")
    || !path_merge(ADMISSIONS_INDEX, index_path, "ADMISSIONS_INDEX")
    || !path_merge(LABEVENTS_INDEX, index_path, "LABEVENTS_INDEX")
    || !path_merge(PATIENTS_INDEX, index_path, "PATIENTS_INDEX")
    || !path_merge(TRANSFERS_INDEX, index_path, "TRANSFERS_INDEX")
    || !path_merge(CHARTEVENTS_INDEX, index_path, "CHARTEVENTS_INDEX"))
        r = -1;
    return r;
}