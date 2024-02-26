#include "featureExtract.h"

void filter_by_itemid(struct Feature *dst, int *index, struct Feature *src, int src_size, struct Feature_ID *features, int feature_size, struct StaticInformation *sInfor)
{
    int i = 0, j = 0;
    for (i = 0; i < src_size; i++)
    {
        // 过滤的同时顺便更新起止时间
        if (sInfor->begintime > src[i].time || sInfor->begintime == 0)
            sInfor->begintime = src[i].time;
        if (sInfor->endtime < src[i].time || sInfor->endtime == 0)
            sInfor->endtime = src[i].time;
        // 过滤
        for (j = 0; j < feature_size; j++)
            if (is_in_array(src[i].ITEMID, features[j].ITEMID))
            {
                dst[*index].ITEMID = src[i].ITEMID;
                dst[*index].time = src[i].time;
                dst[*index].value = src[i].value;
                (*index)++;
                break;
            }
    }
}

int feature_cmp(const void *a, const void *b)
{
    struct Feature *pa = (struct Feature *)a;
    struct Feature *pb = (struct Feature *)b;
    long long ta = pa->time;
    long long tb = pb->time;
    return (int)(ta - tb);
}

void feature_extract(int HADMID, int SUBJECT_ID, int *r_size, struct Feature **result, struct Feature_ID *features, int feature_size, struct StaticInformation *sInfor)
{
    struct Feature *features_r = NULL;
    int features_size = 0;
    EVENTS_read(FEATURES_TABLE, HADMID, &features_r, &features_size);

    *result = (struct Feature *)malloc((features_size) * sizeof(struct Feature));
    int result_index = 0;
    // 遍历选出的所有特征数据, 根据要求的特征ID进行筛选
    filter_by_itemid(*result, &result_index, features_r, features_size, features, feature_size, sInfor);
    *r_size = result_index;

    // 排序
    qsort(*result, result_index, sizeof(struct Feature), feature_cmp);

    if (features_r != NULL)
        free(features_r);
}