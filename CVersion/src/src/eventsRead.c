#include "eventsRead.h"

// LABEVENTS与CHARTEVENTS表的读取

/**
 * 插入
 */
void insert_EVENT_NODE(struct EVENTS_DATA_TABLE *data, struct EVENTS_DATA node)
{
    // 在哈希表中找到可以插入的位置, 生成或插入链表
    int index = node.HADM_ID % hash_table_len;
    while (data[index].HADM_ID != node.HADM_ID && data[index].HADM_ID != 0)
    {
        index++;
        if (index >= hash_table_len)
            index %= hash_table_len;
    }
    // 在链表头部插入
    // 新建节点
    struct EVENTS_DATA *new_node = (struct EVENTS_DATA *)malloc(sizeof(struct EVENTS_DATA));
    // 插入
    new_node->ITEMID = node.ITEMID;
    new_node->HADM_ID = node.HADM_ID;
    new_node->SUBJECT_ID = node.SUBJECT_ID;
    new_node->value = node.value;
    new_node->time = node.time;
    new_node->next = NULL;

    if (data[index].start == NULL) // 当前链表为空
    {
        data[index].HADM_ID = node.HADM_ID;
        data[index].start = new_node;
    }
    else
        data[index].end->next = new_node;
    data[index].end = new_node;
}

/**
 * 读取labevents表csv文件
 */
void LABEVENTS_file_read(FILE *csv_file, struct EVENTS_DATA_TABLE *data)
{
    const int BUFFER_MAX = 1024;
    char buffer[BUFFER_MAX];
    fgets(buffer, BUFFER_MAX, csv_file);                // 跳过表头
    while (fgets(buffer, BUFFER_MAX, csv_file) != NULL) // 读取一行数据直到文件结尾
    {
        unsigned int index = 0;
        struct EVENTS_DATA temp_node;

        // 依次读取每一个字段
#ifdef MIMICIII
        buff_index_move(buffer, &index, 1);
        temp_node.SUBJECT_ID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.HADM_ID = my_atoi(&buffer[index]);
#elif defined MIMICIV
        buff_index_move(buffer, &index, 1);
        temp_node.SUBJECT_ID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.HADM_ID = my_atoi(&buffer[index]);
#endif
        // 过滤非当前进程的ID
        if (is_id_in_list(HADM_IDs, HADM_IDs_size, temp_node.HADM_ID) == 0)
            continue;
#ifdef MIMICIII
        buff_index_move(buffer, &index, 1);
        temp_node.ITEMID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.time = str_2_time_stamp(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        index++;
        temp_node.value = my_atof(&buffer[index]);
#elif defined MIMICIV
        buff_index_move(buffer, &index, 2);
        temp_node.ITEMID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.time = str_2_time_stamp(&buffer[index]);
        buff_index_move(buffer, &index, 2);
        temp_node.value = my_atof(&buffer[index]);
#endif

        // 插入
        insert_EVENT_NODE(data, temp_node);
    }
}

void CHARTEVENTS_file_read(FILE *csv_file, struct EVENTS_DATA_TABLE *data)
{
    const int BUFFER_MAX = 1024;
    char buffer[BUFFER_MAX];
    fgets(buffer, BUFFER_MAX, csv_file);                // 跳过表头
    while (fgets(buffer, BUFFER_MAX, csv_file) != NULL) // 读取一行数据直到文件结尾
    {
        unsigned int index = 0;
        struct EVENTS_DATA temp_node;

        //   依次读取每一个字段
#ifdef MIMICIII
        buff_index_move(buffer, &index, 1);
        temp_node.SUBJECT_ID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.HADM_ID = my_atoi(&buffer[index]);
#elif defined MIMICIV
        temp_node.SUBJECT_ID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.HADM_ID = my_atoi(&buffer[index]);
#endif

        // 若不是本进程需要读取的数据则跳过
        if (is_id_in_list(HADM_IDs, HADM_IDs_size, temp_node.HADM_ID) == 0)
            continue;
#ifdef MIMICIII
        buff_index_move(buffer, &index, 2);
        temp_node.ITEMID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.time = str_2_time_stamp(&buffer[index]);
        buff_index_move(buffer, &index, 3);
        index++;
        temp_node.value = my_atof(&buffer[index]);
#elif defined MIMICIV
        buff_index_move(buffer, &index, 2);
        temp_node.time = str_2_time_stamp(&buffer[index]);
        buff_index_move(buffer, &index, 2);
        temp_node.ITEMID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.value = my_atof(&buffer[index]);
#endif
        // 插入
        insert_EVENT_NODE(data, temp_node);
    }
}

void OUTPUTEVENTS_file_read(FILE *csv_file, struct EVENTS_DATA_TABLE *data)
{
    const int BUFFER_MAX = 1024;
    char buffer[BUFFER_MAX];
    fgets(buffer, BUFFER_MAX, csv_file);                // 跳过表头
    while (fgets(buffer, BUFFER_MAX, csv_file) != NULL) // 读取一行数据直到文件结尾
    {
        unsigned int index = 0;
        struct EVENTS_DATA temp_node;

//   依次读取每一个字段
#ifdef MIMICIII
        buff_index_move(buffer, &index, 1);
        temp_node.SUBJECT_ID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.HADM_ID = my_atoi(&buffer[index]);
#elif defined MIMICIV
        temp_node.SUBJECT_ID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.HADM_ID = my_atoi(&buffer[index]);
#endif

        // 若不是本进程需要读取的数据则跳过
        if (is_id_in_list(HADM_IDs, HADM_IDs_size, temp_node.HADM_ID) == 0)
            continue;
#ifdef MIMICIII
        buff_index_move(buffer, &index, 2);
        temp_node.time = str_2_time_stamp(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.ITEMID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.value = my_atof(&buffer[index]);
#elif defined MIMICIV
        buff_index_move(buffer, &index, 2);
        temp_node.time = str_2_time_stamp(&buffer[index]);
        buff_index_move(buffer, &index, 2);
        temp_node.ITEMID = my_atoi(&buffer[index]);
        buff_index_move(buffer, &index, 1);
        temp_node.value = my_atof(&buffer[index]);
#endif
        // 插入
        insert_EVENT_NODE(data, temp_node);
    }
}

/**
 * 查找, 查找所有匹配的数据并返回
 */
void EVENTS_read(struct EVENTS_DATA_TABLE *data, unsigned int HADM_ID, struct Feature **result, int *r_size)
{
    int start_index = HADM_ID % hash_table_len;
    int index = start_index;
    (*r_size) = 0;
    if (data[index].HADM_ID == 0)
        return; // 无对应ID数据
    while (data[index].HADM_ID != HADM_ID)
    {
        index++;
        if (index >= hash_table_len)
            index %= hash_table_len;
        else if (index == start_index)
            return; // 找不到, 退出
    }

    struct EVENTS_DATA *search_ptr = data[index].start;
    // 找到, 寻找重复行
    while (search_ptr->HADM_ID == HADM_ID && search_ptr->next != NULL)
    {
        search_ptr = search_ptr->next;
        (*r_size)++;
    }
    *result = (struct Feature *)malloc((*r_size) * sizeof(struct Feature));

    search_ptr = data[index].start;
    int i = 0;
    for (i = 0; i < (*r_size); i++)
    {
        // 复制数据
        (*result)[i].ITEMID = search_ptr->ITEMID;
        (*result)[i].time = search_ptr->time;
        (*result)[i].value = search_ptr->value;
        // 下一个节点
        search_ptr = search_ptr->next;
    }
}

void LABEVENTS_table_read()
{
    FILE *csv_file = fopen(LABEVENTS, "r");
    LABEVENTS_file_read(csv_file, FEATURES_TABLE);
    fclose(csv_file);
}

void CHARTEVENTS_table_read()
{
    FILE *csv_file = fopen(CHARTEVENTS, "r");
    CHARTEVENTS_file_read(csv_file, FEATURES_TABLE);
    fclose(csv_file);
}

void OUTPUTEVENTS_table_read()
{
    FILE *csv_file = fopen(OUTPUTEVENTS, "r");
    OUTPUTEVENTS_file_read(csv_file, FEATURES_TABLE);
    fclose(csv_file);
}