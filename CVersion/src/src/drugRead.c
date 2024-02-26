#include "drugRead.h"

/**
 * 提取相关信息
 */
void drug_row_info_extract(char *buff, struct DRUG_DATA *dr)
{
    // 初始化
    char STARTDATE[64];
    char ENDDATE[64];

    unsigned int index = 0;
    int len = 0;

#ifdef MIMICIII
    buff_index_move(buff, &index, 1);
    dr->SUBJECT_ID = my_atoi(&buff[index]);
    buff_index_move(buff, &index, 1);
    dr->HADM_ID = my_atoi(&buff[index]);
    // 跳过非本进程的ID
    if (is_id_in_list(HADM_IDs, HADM_IDs_size, dr->HADM_ID) == 0)
    {
        // 将时间设置为-1, 该数据会被判定为异常数据跳过
        dr->STARTDATE = -1;
        dr->ENDDATE = -1;
        return;
    }

    dr->PROD_STRENGTH = (char *)malloc(256);

    // 读取STARTDATE
    buff_index_move(buff, &index, 2);
    str_cpy(buff, STARTDATE, &index, ',');
    // 读取ENDDATE
    index += 1;
    str_cpy(buff, ENDDATE, &index, ',');
    // 读取GSN
    // GSN字段可能为空
    buff_index_move(buff, &index, 6);
    dr->GSN = 0;
    if (buff[index] != ',')
    {
        index += 1; // 有数字, 跳过"符号
        while (buff[index] != '"')
        {
            dr->GSN = (dr->GSN * 10) + (buff[index] - '0');
            index++;
        }
    }
    // 读取NDC
    // NDC字段不为空, 无数字时会有"0"填充
    // 读取GSN字段后可能会留下一个"符号, 也可能GSN字段为空, 故直接使用buff_index_move函数来移动
    buff_index_move(buff, &index, 1);
    index += 1; // 跳过跳过"符号
    dr->NDC = 0;
    while (buff[index] != '"')
    {
        dr->NDC = (dr->NDC * 10) + (buff[index] - '0');
        index++;
    }
    // 读取PROD_STRENGTH
    index += 3; // 跳过残留的",分隔符,和第一个"符号
    str_cpy(buff, dr->PROD_STRENGTH, &index, '"');
#elif defined MIMICIV
    dr->SUBJECT_ID = my_atoi(&buff[index]);
    buff_index_move(buff, &index, 1);
    dr->HADM_ID = my_atoi(&buff[index]);
    // 跳过非本进程的ID
    if (is_id_in_list(HADM_IDs, HADM_IDs_size, dr->HADM_ID) == 0)
    {
        // 将时间设置为-1, 该数据会被判定为异常数据跳过
        dr->STARTDATE = -1;
        dr->ENDDATE = -1;
        return;
    }

    dr->PROD_STRENGTH = (char *)malloc(256);
    //  读取STARTDATE
    buff_index_move(buff, &index, 2);
    str_cpy(buff, STARTDATE, &index, ',');
    // 读取ENDDATE
    index += 1;
    str_cpy(buff, ENDDATE, &index, ',');
    // 读取GSN
    // GSN字段可能为空
    buff_index_move(buff, &index, 3);
    dr->GSN = 0;
    while (buff[index] != ',')
    {
        dr->GSN = (dr->GSN * 10) + (buff[index] - '0');
        index++;
    }
    index += 1; // 跳过跳过,分隔符
    dr->NDC = 0;
    while (buff[index] != ',')
    {
        dr->NDC = (dr->NDC * 10) + (buff[index] - '0');
        index++;
    }
    // 读取PROD_STRENGTH
    index += 1; // 跳过残留的,分隔符
    str_cpy(buff, dr->PROD_STRENGTH, &index, ',');
#endif
    dr->STARTDATE = str_2_time_stamp(STARTDATE);
    dr->ENDDATE = str_2_time_stamp(ENDDATE);
    // 计算结束时间与开始时间之间的差值(小时)
    dr->DIFF = (int)((dr->ENDDATE - dr->STARTDATE) / 3600);
}

void insert_DRUG_NODE(struct DRUG_DATA_TABLE *data, struct DRUG_DATA node)
{
    int index = node.HADM_ID % hash_table_len;
    while (data[index].HADM_ID != node.HADM_ID && data[index].HADM_ID != 0)
    {
        index++;
        if (index >= hash_table_len)
            index %= hash_table_len;
    }

    struct DRUG_DATA *new_node = (struct DRUG_DATA *)malloc(sizeof(struct DRUG_DATA));
    if (data[index].start == NULL)
    {
        data[index].start = new_node;
        data[index].HADM_ID = node.HADM_ID;
    }
    else
        data[index].end->next = new_node;
    data[index].end = new_node;
    // 复制数据
    new_node->STARTDATE = node.STARTDATE;
    new_node->ENDDATE = node.ENDDATE;
    new_node->PROD_STRENGTH = node.PROD_STRENGTH;
    new_node->GSN = node.GSN;
    new_node->NDC = node.NDC;
    new_node->DIFF = node.DIFF;
    new_node->HADM_ID = node.HADM_ID;
    new_node->SUBJECT_ID = node.SUBJECT_ID;
    new_node->next = NULL;
    return;
}

/**
 * 读取整个处方信息表到内存中
 */
void PRESCRIPTION_file_read(FILE *csv_file, struct DRUG_DATA_TABLE *data, unsigned int *data_size)
{
    const int BUFFER_MAX = 1024;
    char buffer[BUFFER_MAX];
    fgets(buffer, BUFFER_MAX, csv_file);                // 跳过表头
    while (fgets(buffer, BUFFER_MAX, csv_file) != NULL) // 读取一行数据直到文件结尾
    {
        struct DRUG_DATA temp;
        drug_row_info_extract(buffer, &temp);
        // 存在无时间的数据, 需要跳过
        if (temp.STARTDATE <= 0 || temp.ENDDATE <= 0)
            continue;
        // 插入
        insert_DRUG_NODE(data, temp);
        *data_size += 1;
    }
}

void PRESCRIPTION_table_read()
{
    FILE *csv_file = fopen(PRESCRIPTION, "r");

    PRESCRIPTION_TABLE = (struct DRUG_DATA_TABLE *)malloc(hash_table_len * sizeof(struct DRUG_DATA_TABLE));
    memset(PRESCRIPTION_TABLE, 0, hash_table_len * sizeof(struct DRUG_DATA_TABLE));

    unsigned int data_size = 0;
    PRESCRIPTION_file_read(csv_file, PRESCRIPTION_TABLE, &data_size);
    fclose(csv_file);
}
