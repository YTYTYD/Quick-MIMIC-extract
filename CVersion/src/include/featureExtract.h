#pragma once
#include "drugExtract.h"
#include "eventsRead.h"

// 需要提取的特征
struct Feature_ID
{
    char name[256];
    int ITEMID[256];
};

void filter_by_itemid(struct Feature *dst, int *index, struct Feature *src, int src_size, struct Feature_ID *features, int feature_size, struct StaticInformation *sInfor);
int feature_cmp(const void *a, const void *b);
void feature_extract(int HADMID, int SUBJECT_ID, int *r_size, struct Feature **result, struct Feature_ID *features, int feature_size, struct StaticInformation *sInfor);