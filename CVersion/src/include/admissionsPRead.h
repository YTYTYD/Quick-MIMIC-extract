#pragma once
#include <stdio.h>
#include <utils.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

extern char ADMISSIONS[512];
extern const int ADMISSION_LEN;
extern struct ADMISSIONS_DATA *ADMISSIONS_TABLE;

// admissions表读取并存储在内存中
struct ADMISSIONS_DATA
{
    unsigned int SUBJECT_ID;
    unsigned int HADM_ID;
    long long ADMITIME;
    int ADMIYEAR;
    struct ADMISSIONS_DATA *left;
    struct ADMISSIONS_DATA *right;
};

/**
 * 插入
 */
void insert_ADMISSIONS_NODE(struct ADMISSIONS_DATA *data, struct ADMISSIONS_DATA node, unsigned int *data_size);

/**
 * 读取csv文件
 */
void ADMISSIONS_file_read(FILE *csv_file, struct ADMISSIONS_DATA *data, unsigned int *data_size);

/**
 * 查找, 找到返回0, 未找到返回-1
 */
int find_ADMISSIONS_NODE(struct ADMISSIONS_DATA *data, unsigned int HADM_ID, struct ADMISSIONS_DATA **result);

void ADMISSIONS_table_read();