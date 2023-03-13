/**
 * 根据索引读取医嘱与日期
 */
void read_text(FILE *NOTEEVENT, unsigned int offset, struct NOTE *result)
{
    char *DATE = (char *)malloc(64);            // 日期
    result->TEXT = (char *)malloc(1024 * 1024); // 医嘱文本, 有些巨长...
    const int BUFFER_SIZE = 1024;
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char)); // 单行读取缓存

    fseeko64(NOTEEVENT, offset, SEEK_SET); //??? 用64干嘛???

    fgets(buffer, BUFFER_SIZE, NOTEEVENT);

    unsigned int index = 0;
    buff_index_move(buffer, &index, 3);
    str_cpy(buffer, DATE, &index, ',');
    DATE = strcat(DATE, " 23:00:00"); // 每天的最后一个小时
    result->DATE = str_2_time_stamp(DATE);
    buff_index_move(buffer, &index, 7);
    index += 1; // 跳过第一个"号

    // 开始读取医嘱文本
    fseeko64(NOTEEVENT, offset + index, SEEK_SET);
    char read_char;
    unsigned count = 0;
    read_char = fgetc(NOTEEVENT);
    while (read_char != '\"')
    {
        result->TEXT[count] = read_char;
        count += 1;

        read_char = fgetc(NOTEEVENT);
    }
    result->TEXT[count] = '\0';
    free(buffer);
    free(DATE);
}

int note_cmp(const void *a, const void *b)
{
    struct NOTE *pa = (struct NOTE *)a;
    struct NOTE *pb = (struct NOTE *)b;
    long long na = pa->DATE;
    long long nb = pb->DATE;
    return (int)(na - nb);
}

// 做为最后一个小时的医嘱数据
void note_extract(int HADM_ID, struct NOTE **results, int *r_size)
{
    FILE *fptr = fopen(NOTEEVENT, "r");

    const int ROWSMAXSIZE = 2048; // 设定的最大行数
    unsigned int row_offset[ROWSMAXSIZE];
    unsigned int result_size = 0;
    // 读取所有索引
    result_size = get_all_offset_m(HADM_ID, NOTEEVENT_M_INDEX, row_offset, 1851344);
    *results = (struct NOTE *)malloc(result_size * sizeof(struct NOTE));
    memset(*results, 0, sizeof(struct NOTE) * result_size);

    // 根据索引逐个查找
    int i = 0;
    for (i = 0; i < result_size; i++)
        read_text(fptr, row_offset[i], &((*results)[i]));

    // 排序
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
    fclose(fptr);
    for (i = index_1 + 1; i < result_size; i++)
        free((*results)[i].TEXT);
}