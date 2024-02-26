#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "idExtract.h"
#include "init.h"

// LABEVENTS与CHARTEVENTS表的读取
extern struct ID_node *HADM_IDs;
extern int HADM_IDs_size;
extern char LABEVENTS[512];
extern char CHARTEVENTS[512];
extern char OUTPUTEVENTS[512];
extern struct EVENTS_DATA_TABLE *FEATURES_TABLE;

struct EVENTS_DATA
{
    int ITEMID;
    double value;
    long long time;
    int HADM_ID;
    int SUBJECT_ID;
    struct EVENTS_DATA *next;
};

struct EVENTS_DATA_TABLE
{
    struct EVENTS_DATA *start; // 链表开头
    struct EVENTS_DATA *end;   // 链表结尾
    int HADM_ID;
};

// 取出的特征ID与数值
struct Feature
{
    int ITEMID;
    double value;
    long long time;
};


void insert_EVENT_NODE(struct EVENTS_DATA_TABLE *data, struct EVENTS_DATA node);
void LABEVENTS_file_read(FILE *csv_file, struct EVENTS_DATA_TABLE *data);
void CHARTEVENTS_file_read(FILE *csv_file, struct EVENTS_DATA_TABLE *data);
void OUTPUTEVENTS_file_read(FILE *csv_file, struct EVENTS_DATA_TABLE *data);
void EVENTS_read(struct EVENTS_DATA_TABLE *data, unsigned int HADM_ID, struct Feature **result, int *r_size);
void LABEVENTS_table_read();
void CHARTEVENTS_table_read();
void OUTPUTEVENTS_table_read();