#pragma once
#include "mpi.h"
#include "utils.h"
#include "icdCodes.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

extern const int ID_LEN;
extern char DIAGNOSES_ICD[512];

struct ID_node
{
    int HADM_ID;
    int SUBJECT_ID;
    int ICD_CODE;
};


/**
 * 从一行数据中分离出HADM_ID和SUBJECT_ID
 */
void get_HID_SID(char *buff, int *HID, int *SID, int *ICD_CODE);

/**
 * 判断给定HID是否在列表中
 */
int is_id_in_list(struct ID_node *list, int list_size, int HADM_ID);

int id_cmp(const void *a, const void *b);

void id_extract(struct ID_node **result, int *r_size, int MPI_rank, int MPI_size);