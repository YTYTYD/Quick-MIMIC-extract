#include "patientsPRead.h"

/**
 * 插入
 */
void insert_PATIENTS_NODE(struct PATIENTS_DATA *data, struct PATIENTS_DATA node, unsigned int *data_size)
{
    struct PATIENTS_DATA *c_index = data; // 当前节点
    while (c_index->SUBJECT_ID != 0)
    { // 搜索到可插入的叶子节点
        if (c_index->SUBJECT_ID <= node.SUBJECT_ID)
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
    data[*data_size].GENDER = node.GENDER;
#ifdef MIMICIII
    data[*data_size].YOB = node.YOB;
#elif defined MIMICIV
    data[*data_size].AGE = node.AGE;
#endif
    (*data_size)++;
    return;
}

int find_PATIENTS_NODE(struct PATIENTS_DATA *data, unsigned int SUBJECT_ID, struct PATIENTS_DATA **result)
{
    while (data->SUBJECT_ID != 0)
    {
        if (data->SUBJECT_ID == SUBJECT_ID)
        {
            // 找到, 返回
            *result = data;
            return 0;
        }
        if (data->SUBJECT_ID > SUBJECT_ID)
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

void PATIENTS_table_read()
{
    FILE *csv_file = fopen(PATIENTS, "r");

    // 申请内存
    PATIENTS_TABLE = (struct PATIENTS_DATA *)malloc(PATIENTS_LEN * sizeof(struct PATIENTS_DATA));
    unsigned int data_size = 0;
    memset(PATIENTS_TABLE, 0, PATIENTS_LEN * sizeof(struct PATIENTS_DATA));

    // 读取csv文件并
    char buffer[1024];
    fgets(buffer, 1024, csv_file); // 表头读取
    while (fgets(buffer, 1024, csv_file) != NULL)
    {
        // 解析数据
        unsigned int index = 0;
        struct PATIENTS_DATA temp_node;
#ifdef MIMICIII
        buff_index_move(buffer, &index, 1);
        temp_node.SUBJECT_ID = my_atoi(&buffer[index]);
        // 读取性别信息
        buff_index_move(buffer, &index, 1);
        index += 1;
        if (buffer[index] == 'F')
            temp_node.GENDER = 1;
        else
            temp_node.GENDER = 0;

        // 读取入院年份
        buff_index_move(buffer, &index, 1);
        temp_node.YOB = my_atoi(&buffer[index]);
#elif defined MIMICIV
        temp_node.SUBJECT_ID = my_atoi(buffer);
        // 读取性别信息
        buff_index_move(buffer, &index, 1);
        if (buffer[index] == 'F')
            temp_node.GENDER = 1;
        else
            temp_node.GENDER = 0;

        // 读取年龄
        buff_index_move(buffer, &index, 1);
        temp_node.AGE = my_atoi(buffer + index);
#endif
        // 插入
        insert_PATIENTS_NODE(PATIENTS_TABLE, temp_node, &data_size);
    }

    fclose(csv_file);
}