#include "drugExtract.h"

// 按开始时间从小到大排序
int drug_cmp(const void *a, const void *b)
{
    struct DRUG *pa = (struct DRUG *)a;
    struct DRUG *pb = (struct DRUG *)b;
    long long na = pa->STARTDATE;
    long long nb = pb->STARTDATE;
    return (int)(na - nb);
}

void drug_extract(int HADMID, struct DRUG **drug_rows, int *r_size, struct StaticInformation *sInfo)
{
    struct DRUG_DATA_TABLE *ptr = PRESCRIPTION_TABLE;
    (*r_size) = 0;

    int index = HADMID % hash_table_len;
    int f_index = index;
    if (ptr[f_index].HADM_ID == 0)
        return;
    while (ptr[f_index].HADM_ID != HADMID)
    {
        f_index++;
        if (f_index >= hash_table_len)
            f_index %= hash_table_len;
        if (f_index == index)
            return;
    }

    struct DRUG_DATA *search_ptr = ptr[f_index].start;
    // 找到, 寻找重复行
    while (search_ptr->next != NULL)
    {
        search_ptr = search_ptr->next;
        (*r_size)++;
    }

    *drug_rows = (struct DRUG *)malloc((*r_size) * sizeof(struct DRUG));
    memset(*drug_rows, 0, sizeof(struct DRUG) * (*r_size));

    search_ptr = ptr[f_index].start;
    int i = 0;
    for (i = 0; i < (*r_size); i++)
    {
        // 复制数据
        (*drug_rows)[i].DIFF = search_ptr->DIFF;
        (*drug_rows)[i].STARTDATE = search_ptr->STARTDATE;
        (*drug_rows)[i].ENDDATE = search_ptr->ENDDATE;
        (*drug_rows)[i].PROD_STRENGTH = search_ptr->PROD_STRENGTH;
        (*drug_rows)[i].GSN = search_ptr->GSN;
        (*drug_rows)[i].NDC = search_ptr->NDC;
        // 更新起止时间
        if ((*drug_rows)[i].STARTDATE < sInfo->begintime || sInfo->begintime == 0)
            sInfo->begintime = (*drug_rows)[i].STARTDATE;
        if ((*drug_rows)[i].ENDDATE > (*sInfo).endtime || sInfo->endtime == 0)
            sInfo->endtime = (*drug_rows)[i].ENDDATE;
        // 下一个节点
        search_ptr = search_ptr->next;
    }
    // 按给药开始时间排序
    qsort(*drug_rows, (*r_size), sizeof((*drug_rows)[0]), drug_cmp);
}