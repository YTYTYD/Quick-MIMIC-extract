#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

extern char TRANSFERS[512];
extern const int TRANSFERS_LEN;
extern struct TRANSFER_DATA *TRANSFERS_TABLE;

// TRANSFERS表读取并存储在内存中
struct TRANSFER_DATA
{
    unsigned int SUBJECT_ID;
    unsigned int HADM_ID;
    int UNIT1;
    int UNIT2;
    struct TRANSFER_DATA *left;
    struct TRANSFER_DATA *right;
};

void insert_TRANSFER_NODE(struct TRANSFER_DATA *data, struct TRANSFER_DATA node, unsigned int *data_size);
void TRANSFER_file_read(FILE *csv_file, struct TRANSFER_DATA *data, unsigned int *data_size);
int find_TRANSFER_NODE(struct TRANSFER_DATA *data, unsigned int HADM_ID, int *UNIT1, int *UNIT2);
void TRANSFER_table_read();