#include <init.h>

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
        printf("%s not exist\n", file_name);
        return 0;
    }
    return 1;
}

int init(int mpi_size)
{
    if (mpi_size <= 1)
    {
        printf("mpisize must > 1\n");
        return -1;
    }
    int r = 0;
    if (access(OUTPUT_DIR,0) != 0){
        printf("output path not exist\n");
        r = -1;
    }
    //依次检查每个文件是否存在
#ifdef MIMICIV
    if (!path_merge(DIAGNOSES_ICD, data_path, "diagnoses_icd.csv")
    || !path_merge(PRESCRIPTION, data_path, "prescriptions.csv")
    || !path_merge(PATIENTS, data_path, "patients.csv")
    || !path_merge(ADMISSIONS, data_path, "admissions.csv")
    || !path_merge(LABEVENTS, data_path, "labevents.csv")
    || !path_merge(CHARTEVENTS, data_path, "chartevents.csv")
    || !path_merge(ICUSTAYS, data_path, "icustays.csv")
    || !path_merge(OUTPUTEVENTS, data_path, "outputevents.csv"))
        r = -1;
#elif defined(MIMICIII)
    if (!path_merge(DIAGNOSES_ICD, data_path, "DIAGNOSES_ICD.csv")
    || !path_merge(PRESCRIPTION, data_path, "PRESCRIPTIONS.csv")
    || !path_merge(NOTEEVENT, data_path, "NOTEEVENTS.csv")
    || !path_merge(PATIENTS, data_path, "PATIENTS.csv")
    || !path_merge(ADMISSIONS, data_path, "ADMISSIONS.csv")
    || !path_merge(TRANSFERS, data_path, "TRANSFERS.csv")
    || !path_merge(LABEVENTS, data_path, "LABEVENTS.csv")
    || !path_merge(CHARTEVENTS, data_path, "CHARTEVENTS.csv")
    || !path_merge(OUTPUTEVENTS, data_path, "OUTPUTEVENTS.csv"))
        r = -1;
#endif
    return r;
}