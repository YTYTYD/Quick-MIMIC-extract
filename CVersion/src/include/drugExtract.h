#pragma once
#include <stdio.h>
#include <utils.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "sInfoExtract.h"
#include "drugRead.h"

extern struct DRUG_DATA_TABLE *PRESCRIPTION_TABLE;

// 用药信息
struct DRUG
{
    long long STARTDATE; // 用药开始时间 2175-06-11 00:00:00
    long long ENDDATE;   // 用药结束时间 2175-06-12 00:00:00
    char *PROD_STRENGTH; // 用药强度, 例如:1mg Capsule
    int GSN;             // 通用序列号
    long long NDC;       // 国家药品编码
    int DIFF;            // 开始与结束时间之间的差值(单位:小时)
};

int drug_cmp(const void *a, const void *b);
void drug_extract(int HADMID, struct DRUG **drug_rows, int *r_size, struct StaticInformation *sInfo);