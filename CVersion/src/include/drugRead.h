#pragma once
#include <stdio.h>
#include "defines.h"
#include "utils.h"
#include "idExtract.h"
#include "init.h"

extern struct ID_node *HADM_IDs;
extern int HADM_IDs_size;
extern char PRESCRIPTION[512];
extern struct DRUG_DATA_TABLE *PRESCRIPTION_TABLE;

struct DRUG_DATA
{
    int HADM_ID;
    int SUBJECT_ID;
    long long STARTDATE; // 用药开始时间 2175-06-11 00:00:00
    long long ENDDATE;   // 用药结束时间 2175-06-12 00:00:00
    char *PROD_STRENGTH; // 用药强度, 例如:1mg Capsule
    int GSN;             // 通用序列号
    long long NDC;       // 国家药品编码
    int DIFF;            // 开始与结束时间之间的差值(单位:小时)
    struct DRUG_DATA *next;
};

struct DRUG_DATA_TABLE
{
    struct DRUG_DATA *start;
    struct DRUG_DATA *end;
    int HADM_ID;
};

void drug_row_info_extract(char *buff, struct DRUG_DATA *dr);
void insert_DRUG_NODE(struct DRUG_DATA_TABLE *data, struct DRUG_DATA node);
void PRESCRIPTION_file_read(FILE *csv_file, struct DRUG_DATA_TABLE *data, unsigned int *data_size);
void PRESCRIPTION_table_read();