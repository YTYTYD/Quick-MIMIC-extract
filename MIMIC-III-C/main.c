#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "structs.h"
#include "consts.h"

#ifdef __linux
#include <sys/time.h>
#endif

#include "utils.c"
#include "init.c"
#include "indexReadUtil.c"
#include "indexRead.c"

// 预先读取到内存中的表
struct ADMISSIONS_DATA *ADMISSIONS_TABLE;
struct PATIENTS_DATA *PATIENTS_TABLE;
#ifdef MIMICIV
struct ICUSTAY_DATA *ICUSTAY_TABLE;
#elif defined(MIMICIII)
struct TRANSFER_DATA *TRANSFERS_TABLE;
#endif

#ifdef MIMICIV
#include "icustaysExtractPRead.c"
#include "featureExtract_iv.c"
#include "admissionsPRead_iv.c"
#include "patientsPRead_iv.c"
#include "idExtract_iv.c"
#include "sInfoExtract_iv.c"
#include "drugExtract_iv.c"
#elif defined(MIMICIII)
#include "transfersPRead.c"
#include "featureExtract_iii.c"
#include "admissionsPRead_iii.c"
#include "patientsPRead_iii.c"
#include "idExtract_iii.c"
#include "sInfoExtract_iii.c"
#include "drugExtract_iii.c"
#include "noteExtract.c"
#endif

struct Feature_ID *feature_ids;
int feature_ids_size = 0;

// 从文件中读取要提取的特征名与ID
void feature_read()
{
    FILE *list = fopen(FEATURES_LIST, "r");
    feature_ids = (struct Feature_ID *)malloc(1024 * sizeof(struct Feature_ID));
    memset(feature_ids, -1, 1024 * sizeof(struct Feature_ID));
    char buff[1024];
    while (fgets(buff, 1024, list) != NULL)
    {
        int index_of_separator = 0;
        for (index_of_separator = 0; index_of_separator < 1024 && buff[index_of_separator] != ','; index_of_separator++)
            ;
        strncpy(feature_ids[feature_ids_size].name, buff, index_of_separator);
        feature_ids[feature_ids_size].name[index_of_separator] = '\0';
        int i = 0;
        for (i = 0; i < feature_ids_size; i++)
        {
            if (strcmp(feature_ids[i].name, feature_ids[feature_ids_size].name) == 0)
            {
                int j = 0;
                while (feature_ids[i].ITEMID[j] != -1)
                    j++;
                feature_ids[i].ITEMID[j] = my_atoi(buff + index_of_separator + 1);
                break;
            }
        }
        if (i >= feature_ids_size)
        {
            feature_ids[feature_ids_size].ITEMID[0] = my_atoi(buff + index_of_separator + 1);
            feature_ids_size++;
        }
    }
    fclose(list);
}

/**
 * 根据两个ID进行提取
 */
void extract(int HADM_ID, int SUBJECT_ID)
{
    // 病人基本信息
    struct StaticInformation sInfor;
    sInfor.begintime = 0;
    sInfor.endtime = 0;
    sInfo_extract(&sInfor, HADM_ID, SUBJECT_ID);
#ifdef MIMICIII
    // 医嘱信息
    struct NOTE *notes;
    int notes_size = 0;
    note_extract(HADM_ID, &notes, &notes_size);
#endif
    // 药物信息
    struct DRUG *drugs;
    int drugs_size = 0;
    drug_extract(HADM_ID, &drugs, &drugs_size, &sInfor);
    // 化验信息
    struct Feature *features;
    int features_size = 0;
    feature_extract(HADM_ID, SUBJECT_ID, &features_size, &features, feature_ids, feature_ids_size, &sInfor);

    // 计算时长
    if (sInfor.begintime > sInfor.HospAdmiTime)
        sInfor.begintime = sInfor.HospAdmiTime;
    sInfor.HospAdmiTime = sInfor.endtime / 3600 - sInfor.begintime / 3600;

    // 输出到csv文件
    char output_path[512];
    strcpy(output_path, OUTPUT_DIR);
    my_itoa(HADM_ID, output_path + strlen(output_path));
    strcpy(output_path + strlen(output_path), ".csv");
    FILE *outputcsv_file = fopen(output_path, "w");

    int i = 0, j = 0, k = 0;
    // 输出表头
    for (i = 0; i < feature_ids_size; i++)
        fprintf(outputcsv_file, "%s,", feature_ids[i].name);
#ifdef MIMICIII
    fprintf(outputcsv_file, "Age,Gender,Unit1,Unit2,HospAdmTime,ICULOS,Strategy,GSN,NDC,TEXT\n");
#elif defined(MIMICIV)
    fprintf(outputcsv_file, "Age,Gender,Unit1,Unit2,HospAdmTime,ICULOS,Strategy,GSN,NDC\n");
#endif

    // 从开始时间到结束时间遍历
    int drug_index = 0, note_index = 0, feature_index = 0;
    for (i = 0; i < sInfor.HospAdmiTime; i++)
    {
        long long ctime = sInfor.begintime / 3600 + (long long)i; // 当前时间(小时)
        // 输出特征
        for (j = 0; j < feature_ids_size; j++)
        {
            for (k = feature_index; k < features_size && features[k].time / 3600 <= ctime; k++)
            {
                if (is_in_array(features[k].ITEMID, feature_ids[j].ITEMID) && features[k].time / 3600 == ctime)
                {
                    fprintf(outputcsv_file, "%f", features[k].value);
                    break;
                }
            }
            fprintf(outputcsv_file, ",");
        }
        while (feature_index < features_size && features[feature_index].time / 3600 <= ctime)
            feature_index++;
        // 输出病人基本信息
        fprintf(outputcsv_file, "%d,%d,%d,%d,%d,%d,", sInfor.Age, sInfor.Gender, sInfor.Util1, sInfor.Util2, sInfor.HospAdmiTime, i + 1);
        // 输出药品信息
        char Strategy[4096] = "\0";
        char GSN[4096] = "\0";
        char NDC[4096] = "\0";
        char trans_buff[32];
        for (drug_index = 0; drug_index < drugs_size; drug_index++)
        {
            if (drugs[drug_index].STARTDATE / 3600 <= ctime && drugs[drug_index].ENDDATE / 3600 >= ctime)
            {
                if (strlen(Strategy) != 0)
                {
                    strcpy(Strategy + strlen(Strategy), ", ");
                    strcpy(GSN + strlen(GSN), ", ");
                    strcpy(NDC + strlen(NDC), ", ");
                }
                strcpy(Strategy + strlen(Strategy), drugs[drug_index].PROD_STRENGTH);
                my_itoa(drugs[drug_index].GSN, trans_buff);
                strcpy(GSN + strlen(GSN), trans_buff);
                sprintf(trans_buff, "%lld", drugs[drug_index].NDC);
                strcpy(NDC + strlen(NDC), trans_buff);
            }
        }
        fprintf(outputcsv_file, "\"%s\",\"%s\",\"%s\"", Strategy, GSN, NDC);
#ifdef MIMICIII
        // 输出医嘱信息
        if (notes[note_index].DATE / 3600 == ctime)
        {
            fprintf(outputcsv_file, "\"%s\"", notes[note_index].TEXT);
            if (note_index < notes_size)
                note_index++;
        }
#endif

        fprintf(outputcsv_file, "\n");
    }

    fclose(outputcsv_file);

    // 释放

#ifdef MIMICIII
    for (i = 0; i < notes_size; i++)
        free(notes[i].TEXT);
    free(notes);
#endif

    // 释放用药强度字符串内存
    for (i = 0; i < drugs_size; i++)
        free(drugs[i].PROD_STRENGTH);
    free(drugs);
    free(features);
}

int main()
{
    if (init() != 0)
        return 0;
    // 读取需要提取的特征ID
    feature_read();

    // 读取部分表到内存中
    ADMISSIONS_table_read();
    PATIENTS_table_read();
#ifdef MIMICIV
    ICUSTAY_table_read();
#elif defined(MIMICIII)
    TRANSFER_table_read();
#endif
    // 读取索引文件到内存中
    index_read();

    struct H_ID_node *HADM_IDs;
    int HADM_IDs_size = 0;
    id_extract(&HADM_IDs, &HADM_IDs_size);

    
    int i = 0;
    for (i = 0; i < HADM_IDs_size; i++)
    {
#ifdef __linux
        printf("\033[K");
        printf("count: %d\n", i + 1);
        printf("\033[1A");
#elif __WIN32
        printf("\rcount: %d", i+1);
#endif
        extract(HADM_IDs[i].HADM_ID, HADM_IDs[i].SUBJECT_ID);
    }

    free(feature_ids);
    free(HADM_IDs);
    // 释放预读到内存中的表
    free(ADMISSIONS_TABLE);
    free(PATIENTS_TABLE);
    free(PRESCRIPTION_M_INDEX);
    free(LABEVENT_M_INDEX);
#ifdef MIMICIV
    free(ICUSTAY_TABLE);
#elif defined(MIMICIII)
    free(NOTEEVENT_M_INDEX);
#endif
    return 0;
}