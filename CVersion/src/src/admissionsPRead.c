#include <admissionsPRead.h>

/**
 * 插入
 */
void insert_ADMISSIONS_NODE(struct ADMISSIONS_DATA *data, struct ADMISSIONS_DATA node, unsigned int *data_size)
{
    struct ADMISSIONS_DATA *c_index = data; // 当前节点
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
    data[*data_size].ADMITIME = node.ADMITIME;
    data[*data_size].ADMIYEAR = node.ADMIYEAR;
    return;
}

/**
 * 读取csv文件
 */
void ADMISSIONS_file_read(FILE *csv_file, struct ADMISSIONS_DATA *data, unsigned int *data_size)
{
    const int BUFFER_MAX = 1024;
    char buffer[BUFFER_MAX];
    fgets(buffer, BUFFER_MAX, csv_file);                // 跳过表头
    while (fgets(buffer, BUFFER_MAX, csv_file) != NULL) // 读取一行数据直到文件结尾
    {
        unsigned int index = 0;
        char temp_buffer[64];
        struct ADMISSIONS_DATA temp_node;
#ifdef MIMICIII
        // 获取SUBJECT_ID
        buff_index_move(buffer, &index, 1);
        temp_node.SUBJECT_ID = my_atoi(buffer);
        // 获取HADM_ID
        buff_index_move(buffer, &index, 1);
        str_cpy(buffer, temp_buffer, &index, ',');
        temp_node.HADM_ID = my_atoi(temp_buffer);
        // 读取住院时间
        buff_index_move(buffer, &index, 1);
        str_cpy(buffer, temp_buffer, &index, ',');
        temp_node.ADMIYEAR = my_atoi(temp_buffer);
        temp_node.ADMITIME = str_2_time_stamp(temp_buffer);
#elif defined MIMICIV
        // 获取SUBJECT_ID
        temp_node.SUBJECT_ID = my_atoi(buffer);
        // 获取HADM_ID
        buff_index_move(buffer, &index, 1);
        str_cpy(buffer, temp_buffer, &index, ',');
        temp_node.HADM_ID = my_atoi(temp_buffer);
        // 读取住院时间
        buff_index_move(buffer, &index, 1);
        str_cpy(buffer, temp_buffer, &index, ',');
        temp_node.ADMIYEAR = my_atoi(temp_buffer);
        temp_node.ADMITIME = str_2_time_stamp(temp_buffer);

#endif
        // 插入
        insert_ADMISSIONS_NODE(data, temp_node, data_size);
        *data_size += 1;
    }
}

/**
 * 查找, 找到返回0, 未找到返回-1
 */
int find_ADMISSIONS_NODE(struct ADMISSIONS_DATA *data, unsigned int HADM_ID, struct ADMISSIONS_DATA **result)
{
    while (data->HADM_ID != 0)
    {
        if (data->HADM_ID == HADM_ID)
        {
            // 找到, 返回
            *result = data;
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

void ADMISSIONS_table_read()
{
    FILE *csv_file = fopen(ADMISSIONS, "r");
    ADMISSIONS_TABLE = (struct ADMISSIONS_DATA *)malloc(ADMISSION_LEN * sizeof(struct ADMISSIONS_DATA));
    unsigned int data_size = 0;
    memset(ADMISSIONS_TABLE, 0, ADMISSION_LEN * sizeof(struct ADMISSIONS_DATA));
    ADMISSIONS_file_read(csv_file, ADMISSIONS_TABLE, &data_size);
    fclose(csv_file);
}