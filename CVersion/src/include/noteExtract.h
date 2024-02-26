#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "noteRead.h"
extern struct NOTE_DATA *NOTEEVENT_TABLE;

// 医嘱
struct NOTE
{
    char *TEXT;     // 医嘱文本
    long long DATE; // 时间, 当天最后一个小时的时间戳
};

int note_cmp(const void *a, const void *b);
void note_extract(int HADM_ID, struct NOTE **results, int *r_size);