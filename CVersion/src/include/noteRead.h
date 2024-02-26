#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idExtract.h"
#include "utils.h"

extern struct ID_node *HADM_IDs;
extern int HADM_IDs_size;
extern char NOTEEVENT[512];
extern const int NOTEEVENT_LEN;
extern struct NOTE_DATA *NOTEEVENT_TABLE;

struct NOTE_DATA
{
    int HADM_ID;
    int SUBJECT_ID;
    char *TEXT;     // 医嘱文本
    long long DATE; // 时间, 当天最后一个小时的时间戳
    struct NOTE_DATA *left;
    struct NOTE_DATA *right;
};

void insert_NOTE_NODE(struct NOTE_DATA *data, struct NOTE_DATA node, unsigned int *data_size);
void NOTE_file_read(FILE *csv_file, struct NOTE_DATA *data, unsigned int *data_size);
int find_NOTE_NODE(struct NOTE_DATA *data, unsigned int HADM_ID, unsigned int *size, struct NOTE_DATA **start_ptr);
void NOTE_table_read();