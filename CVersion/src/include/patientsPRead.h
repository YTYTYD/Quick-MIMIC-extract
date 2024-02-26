#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "utils.h"

extern char PATIENTS[512];
extern const int PATIENTS_LEN;
extern struct PATIENTS_DATA *PATIENTS_TABLE;

// 读取patients表并存储在内存中
struct PATIENTS_DATA
{
    unsigned int SUBJECT_ID;
    int GENDER;
#ifdef MIMICIII
    int YOB; // 出生年份
#elif defined MIMICIV
    int AGE;
#endif
    struct PATIENTS_DATA *left;
    struct PATIENTS_DATA *right;
};

void insert_PATIENTS_NODE(struct PATIENTS_DATA *data, struct PATIENTS_DATA node, unsigned int *data_size);
int find_PATIENTS_NODE(struct PATIENTS_DATA *data, unsigned int SUBJECT_ID, struct PATIENTS_DATA **result);
void PATIENTS_table_read();