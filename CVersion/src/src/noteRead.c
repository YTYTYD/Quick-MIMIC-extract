#include "noteRead.h"

/**
 * 插入
 */
void insert_NOTE_NODE(struct NOTE_DATA *data, struct NOTE_DATA node, unsigned int *data_size)
{
    struct NOTE_DATA *c_index = data; // 当前节点
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
    data[*data_size].TEXT = node.TEXT;
    data[*data_size].DATE = node.DATE;
    return;
}

/**
 * 读取csv文件
 */
void NOTE_file_read(FILE *csv_file, struct NOTE_DATA *data, unsigned int *data_size)
{
    const int BUFFER_MAX = 1024;
    char buffer[BUFFER_MAX];
    fgets(buffer, BUFFER_MAX, csv_file); // 跳过表头
    // 解析文本时的缓冲区
    char DATE[64];                                        // 日期复制缓冲区
    char n_buffer[BUFFER_MAX];                            // 医嘱文本(单行)读取缓冲区
    char *temp_text_buffer = (char *)malloc(1024 * 1024); // 医嘱文本(全部)读入缓冲区

    while (fgets(buffer, BUFFER_MAX, csv_file) != NULL) // 读取一行数据直到文件结尾
    {
        struct NOTE_DATA temp_node;
        unsigned int index = 0;
        // 获取SUBJECT_ID
        buff_index_move(buffer, &index, 1);
        temp_node.SUBJECT_ID = my_atoi(&buffer[index]);
        // 获取HADM_ID
        buff_index_move(buffer, &index, 1);
        temp_node.HADM_ID = my_atoi(&buffer[index]);

        temp_text_buffer[0] = '\0';
        buff_index_move(buffer, &index, 1);
        str_cpy(buffer, DATE, &index, ',');
        strcat(DATE, " 23:00:00"); // 每天的最后一个小时
        temp_node.DATE = str_2_time_stamp(DATE);

        buff_index_move(buffer, &index, 7);
        index += 1; // 跳过第一个"号

        // 开始读取医嘱文本
        strcpy(temp_text_buffer, &buffer[index]);
        while (fgets(n_buffer, BUFFER_MAX, csv_file) != NULL)
        {
            if (strlen(n_buffer) > 1 && n_buffer[0] == '"' && n_buffer[1] == '\n')
            {
                break;
            }
            strcat(temp_text_buffer, n_buffer);
        }

        // 在读取完成后过滤
        // 过滤不需要的ID
        if (is_id_in_list(HADM_IDs, HADM_IDs_size, temp_node.HADM_ID) == 0)
            continue;

        int text_length = strlen(temp_text_buffer) + 1;
        temp_node.TEXT = (char *)malloc(text_length);
        strcpy(temp_node.TEXT, temp_text_buffer);
        //  插入
        insert_NOTE_NODE(data, temp_node, data_size);
        *data_size += 1;
    }
    free(temp_text_buffer);
}

/**
 * 查找
 * 返回数量以及起始位置的指针
 */
int find_NOTE_NODE(struct NOTE_DATA *data, unsigned int HADM_ID, unsigned int *size, struct NOTE_DATA **start_ptr)
{
    while (data->HADM_ID != 0)
    {
        if (data->HADM_ID == HADM_ID)
        {
            // 找到, 返回
            (*start_ptr) = data;
            struct NOTE_DATA *ptr;
            while (data->HADM_ID == HADM_ID && data->right != NULL)
            {
                data = data->right;
                (*size)++;
            }
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

void NOTE_table_read()
{
    FILE *csv_file = fopen(NOTEEVENT, "r");
    NOTEEVENT_TABLE = (struct NOTE_DATA *)malloc(NOTEEVENT_LEN * sizeof(struct NOTE_DATA));
    unsigned int data_size = 0;
    memset(NOTEEVENT_TABLE, 0, NOTEEVENT_LEN * sizeof(struct NOTE_DATA));
    NOTE_file_read(csv_file, NOTEEVENT_TABLE, &data_size);
    fclose(csv_file);
}