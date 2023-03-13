struct index_data
{
    int ID;
    unsigned int offset;
};

struct index_64_data
{
    int ID;
    long long offset;
};

/**
 * 对某个索引文件进行读取
 * @param file_path: 索引文件路径
 * @param target: 需要存储到的数组指针
 * @param len: 索引文件中索引数量
 */
void read_index_file(char *file_path, struct index_data **target, int len)
{
    char buffer[8];
    *target = (struct index_data *)malloc(len * sizeof(struct index_data));
    FILE *index_file = fopen(file_path, "rb");
    int i = 0;
    for (i = 0; i < len; i++)
    {
        fread(buffer, 8, 1, index_file);
        (*target)[i].ID = (buffer[0] & 0xff) + ((buffer[1] & 0xff) << 8) + ((buffer[2] & 0xff) << 16) + ((buffer[3] & 0xff) << 24);
        (*target)[i].offset = (buffer[4] & 0xff) + ((buffer[5] & 0xff) << 8) + ((buffer[6] & 0xff) << 16) + ((buffer[7] & 0xff) << 24);
    }
    fclose(index_file);
}

/**
 * 对某个索引文件进行读取(64位索引版)
 * @param file_path: 索引文件路径
 * @param target: 需要存储到的数组指针
 * @param len: 索引文件中索引数量
 */
void read_index_file_64(char *file_path, struct index_64_data **target, int len)
{
    char buffer[9];
    *target = (struct index_64_data *)malloc(len * sizeof(struct index_64_data));
    FILE *index_file = fopen(file_path, "rb");
    int i = 0;
    for (i = 0; i < len; i++)
    {
        fread(buffer, 9, 1, index_file);
        (*target)[i].ID = (buffer[0] & 0xff) + ((buffer[1] & 0xff) << 8) + ((buffer[2] & 0xff) << 16) + ((buffer[3] & 0xff) << 24);
        (*target)[i].offset = (long long)(buffer[4] & 0xff) + ((long long)(buffer[5] & 0xff) << 8) + ((long long)(buffer[6] & 0xff) << 16) + ((long long)(buffer[7] & 0xff) << 24) + ((long long)(buffer[8] & 0xff) << 32);
    }
    fclose(index_file);
}

/**
 * 在给定的索引数组中查找所有与id值对应的文件偏移量
 * @param id: 要查找的HADM_ID或SUBJECT_ID
 * @param src_index: 给定的索引数组
 * @param result: 结果存储数组, 每一个数字都是偏移量
 * @param max: 索引条数(即对应表中数据行数)
 * @return: 返回查找的索引条数, 对应该ID在该表中的数据行数
 */
unsigned int get_all_offset_m(int id, struct index_data *src_index, unsigned int *result, unsigned int max)
{
    /*二分法找到当前HID对应的位置*/
    unsigned int start = 0, end = max - 1;
    unsigned int current = (start + end) / 2; // 当前指针位置
    while (1)
    {
        if (src_index[current].ID != id)
        {
            if (current == start)
                return 0; // 未找到
            if (src_index[current].ID > id)
                end = current;
            else
                start = current;
            current = (start + end) / 2;
        }
        else // 找到
            break;
    }

    /*左右查找出所有的偏移*/
    unsigned int counter = 0;
    unsigned int index = current;
    while (index >= 0)
    { // 向前查找
        if (src_index[index].ID != id)
            break;
        result[counter] = src_index[index].offset;
        index -= 1;
        counter += 1;
    }
    /*上一步查出来的offset是倒序的, 这里再倒回来*/
    unsigned int i = 0;
    unsigned int temp = 0;
    for (i = 0; i <= ((counter - 1) / 2); i++)
    {
        temp = result[i];
        result[i] = result[counter - 1 - i];
        result[counter - 1 - i] = temp;
    }
    index = current + 1;
    while (index < max)
    { // 向后查找
        if (src_index[index].ID != id)
            break;
        result[counter] = src_index[index].offset;
        index += 1;
        counter += 1;
    }
    return counter;
}

/**
 * 在给定的索引数组中查找所有与id值对应的文件偏移量(64位索引版)
 * @param id: 要查找的HADM_ID或SUBJECT_ID
 * @param src_index: 给定的索引数组
 * @param result: 结果存储数组, 每一个数字都是偏移量
 * @param max: 索引条数(即对应表中数据行数)
 * @return: 返回查找的索引条数, 对应该ID在该表中的数据行数
 */
unsigned int get_all_offset_m_64(int id, struct index_64_data *src_index, long long *result, unsigned int max)
{
    /*二分法找到当前HID对应的位置*/
    unsigned int start = 0, end = max - 1;
    unsigned int current = (start + end) / 2; // 当前指针位置
    while (1)
    {
        if (src_index[current].ID != id)
        {
            if (current == start)
                return 0; // 未找到
            if (src_index[current].ID > id)
                end = current;
            else
                start = current;
            current = (start + end) / 2;
        }
        else // 找到
            break;
    }

    /*左右查找出所有的偏移*/
    unsigned int counter = 0;
    unsigned int index = current;
    while (index >= 0)
    { // 向前查找
        if (src_index[index].ID != id)
            break;
        result[counter] = src_index[index].offset;
        index -= 1;
        counter += 1;
    }
    /*上一步查出来的offset是倒序的, 这里再倒回来*/
    unsigned int i = 0;
    long long temp = 0;
    for (i = 0; i <= ((counter - 1) / 2); i++)
    {
        temp = result[i];
        result[i] = result[counter - 1 - i];
        result[counter - 1 - i] = temp;
    }
    index = current + 1;
    while (index < max)
    { // 向后查找
        if (src_index[index].ID != id)
            break;
        result[counter] = src_index[index].offset;
        index += 1;
        counter += 1;
    }
    return counter;
}