#include "icustaysExtractPRead.h"

/**
 * 插入
 */
void insert_ICUSTAY_NODE(struct ICUSTAY_DATA *data, struct ICUSTAY_DATA node, unsigned int *data_size)
{
    struct ICUSTAY_DATA *c_index = data; // 当前节点
    while (c_index->HADM_ID != 0)
    { // 搜索到可插入的叶子节点
        if (c_index->HADM_ID <= node.HADM_ID)
        {
            if (c_index->right == NULL)
            {
                c_index->right = &data[*data_size];
                break;
            }
            c_index = c_index->right;
        }
        else
        {
            if (c_index->left == NULL)
            {
                c_index->left = &data[*data_size];
                break;
            }
            c_index = c_index->left;
        }
    }
    // 找到空位, 插入
    data[*data_size].SUBJECT_ID = node.SUBJECT_ID;
    data[*data_size].HADM_ID = node.HADM_ID;
    data[*data_size].UNIT1 = node.UNIT1;
    data[*data_size].UNIT2 = node.UNIT2;
    return;
}

/**
 * 读取csv文件
 */
void ICUSTAY_file_read(FILE *csv_file, struct ICUSTAY_DATA *data, unsigned int *data_size)
{
    const int BUFFER_MAX = 1024;
    char buffer[BUFFER_MAX];
    fgets(buffer, BUFFER_MAX, csv_file);                // 跳过表头
    while (fgets(buffer, BUFFER_MAX, csv_file) != NULL) // 读取一行数据直到文件结尾
    {
        unsigned int index = 0;
        char temp_buffer[64];
        struct ICUSTAY_DATA temp_node;
        temp_node.UNIT1 = 0;
        temp_node.UNIT2 = 0;
        // 获取SUBJECT_ID
        temp_node.SUBJECT_ID = my_atoi(buffer);
        // 获取HADM_ID
        buff_index_move(buffer, &index, 1);
        temp_node.HADM_ID = my_atoi(&buffer[index]);
        // 读取病房信息
        buff_index_move(buffer, &index, 3);
        str_cpy(buffer, temp_buffer, &index, ',');
        if (strcmp(temp_buffer, "MICU") == 0 || strcmp(temp_buffer, "Medical Intensive Care Unit (MICU)") == 0 || strcmp(temp_buffer, "Medical/Surgical Intensive Care Unit (MICU/SICU)") == 0)
            temp_node.UNIT1 = 1;
        if (strcmp(temp_buffer, "SICU") == 0 || strcmp(temp_buffer, "Surgical Intensive Care Unit (SICU)") == 0 || strcmp(temp_buffer, "Medical/Surgical Intensive Care Unit (MICU/SICU)") == 0)
            temp_node.UNIT2 = 1;
        // 插入
        insert_ICUSTAY_NODE(data, temp_node, data_size);
        *data_size += 1;
    }
}

/**
 * 查找, 找到返回0, 未找到返回-1
 */
int find_ICUSTAY_NODE(struct ICUSTAY_DATA *data, unsigned int HADM_ID, int *UNIT1, int *UNIT2)
{
    while (data->HADM_ID != 0)
    {
        if (data->HADM_ID == HADM_ID)
        {
            // 找到, 返回
            *UNIT1 = data->UNIT1;
            *UNIT2 = data->UNIT2;
            return 0;
        }
        if (data->HADM_ID > HADM_ID)
            if (data->left == NULL)
                return -1;
            else
                data = data->left;
        else
        {
            if (data->right == NULL)
                return -1;
            else
                data = data->right;
        }
    }
    return -1;
}

void ICUSTAY_table_read()
{
    FILE *csv_file = fopen(ICUSTAYS, "r");
    ICUSTAY_TABLE = (struct ICUSTAY_DATA *)malloc(ICUSTAY_LEN * sizeof(struct ICUSTAY_DATA));
    unsigned int data_size = 0;
    memset(ICUSTAY_TABLE, 0, ICUSTAY_LEN * sizeof(struct ICUSTAY_DATA));
    ICUSTAY_file_read(csv_file, ICUSTAY_TABLE, &data_size);
    fclose(csv_file);
}