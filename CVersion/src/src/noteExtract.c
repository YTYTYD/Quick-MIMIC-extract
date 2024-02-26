#include "noteExtract.h"

int note_cmp(const void *a, const void *b)
{
    struct NOTE *pa = (struct NOTE *)a;
    struct NOTE *pb = (struct NOTE *)b;
    long long na = pa->DATE;
    long long nb = pb->DATE;
    return (int)(na - nb);
}

void note_extract(int HADM_ID, struct NOTE **results, int *r_size)
{
    unsigned int result_size = 0;
    struct NOTE_DATA *start_ptr = NULL;
    find_NOTE_NODE(NOTEEVENT_TABLE, HADM_ID, &result_size, &start_ptr);
    *results = (struct NOTE *)malloc(result_size * sizeof(struct NOTE));
    memset(*results, 0, sizeof(struct NOTE) * result_size);

    // 根据索引逐个查找
    int i = 0;
    for (i = 0; i < result_size; i++)
    {
        // 复制
        (*results)[i].DATE = start_ptr->DATE;
        // 从大表中复制医嘱文本出来
        (*results)[i].TEXT = (char *)malloc(5120 * 1024);
        strcpy((*results)[i].TEXT, start_ptr->TEXT);
        // 下一个节点
        start_ptr = start_ptr->right;
    }

    // 按时间排序
    qsort(*results, result_size, sizeof(*results)[0], note_cmp);

    // 合并同一天医嘱
    int index_1 = 0, index_2 = 1;
    int copy_flag = 0;
    for (index_2 = 1; index_2 < result_size; index_2++)
    {
        if ((*results)[index_1].DATE == (*results)[index_2].DATE)
        {
            // 合并
            (*results)[index_1].TEXT = strcat((*results)[index_1].TEXT, (*results)[index_2].TEXT);
            copy_flag = 1;
        }
        else
        {
            index_1 += 1;
            if (copy_flag)
            {
                // 将index_2处的数据复制到此处;
                (*results)[index_1].DATE = (*results)[index_2].DATE;
                strcpy((*results)[index_1].TEXT, (*results)[index_2].TEXT);
                copy_flag = 0;
                index_2 -= 1;
            }
        }
    }
    if (result_size == 0)
        *r_size = 0;
    else
        *r_size = index_1 + 1;
    // 释放
    for (i = index_1 + 1; i < result_size; i++)
    {
        if ((*results)[i].TEXT != NULL)
            free((*results)[i].TEXT);
    }
}