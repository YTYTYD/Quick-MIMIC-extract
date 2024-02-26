#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

extern char ICUSTAYS[512];
extern const int ICUSTAY_LEN;
extern struct ICUSTAY_DATA *ICUSTAY_TABLE;

struct ICUSTAY_DATA
{
    unsigned int SUBJECT_ID;
    unsigned int HADM_ID;
    int UNIT1;
    int UNIT2;
    struct ICUSTAY_DATA *left;
    struct ICUSTAY_DATA *right;
};

void insert_ICUSTAY_NODE(struct ICUSTAY_DATA *data, struct ICUSTAY_DATA node, unsigned int *data_size);
void ICUSTAY_file_read(FILE *csv_file, struct ICUSTAY_DATA *data, unsigned int *data_size);
int find_ICUSTAY_NODE(struct ICUSTAY_DATA *data, unsigned int HADM_ID, int *UNIT1, int *UNIT2);
void ICUSTAY_table_read();