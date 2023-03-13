void LABEVENTS_read(FILE *csv_file, int HADM_ID, struct Feature **result, int *r_size)
{
    const int MAX_SIZE = 100000;

    // 偏移量获取
    unsigned int *offset = (unsigned int *)malloc(MAX_SIZE * sizeof(unsigned int));
    unsigned int offset_size = 0;
    offset_size = get_all_offset_m(HADM_ID, LABEVENT_M_INDEX, offset, LABEVENTS_LEN);
    *r_size = offset_size;

    *result = (struct Feature *)malloc(offset_size * sizeof(struct Feature));
    memset(*result, 0, offset_size * sizeof(struct Feature));

    // 开始读取
    char buffer[1024];
    char c_temp[256];
    int i = 0;
    int SUBJECT_ID = -1;
    for (i = 0; i < offset_size; i++)
    {
        // 读取一段
        unsigned int index = 0;
        fseek(csv_file, offset[i], SEEK_SET);
        fgets(buffer, 1024, csv_file);

        // 依次读取每一个字段
        buff_index_move(buffer, &index, 1);
        str_cpy(buffer, c_temp, &index, ',');
        SUBJECT_ID = my_atoi(c_temp);
        index += 1;
        buff_index_move(buffer, &index, 1);
        str_cpy(buffer, c_temp, &index, ',');
        (*result)[i].ITEMID = my_atoi(c_temp);
        index += 1;
        str_cpy(buffer, c_temp, &index, ',');
        (*result)[i].time = str_2_time_stamp(c_temp);
        index += 1;

        str_cpy_2(buffer, c_temp, &index, ',', '\"');
        (*result)[i].value = my_atof(c_temp);
    }

    free(offset);
}

void CHARTEVENTS_read(FILE *index_file, int HADM_ID, int SUBJECT_ID, FILE *csv_file, struct Feature **result, int *r_size)
{
    const int MAX_SIZE = 1000000; // 目前最大接近80000

    // 获取所用偏移量
    long long *offset = (long long *)malloc(MAX_SIZE * sizeof(long long));
    unsigned int offset_size = get_all_offset_64(HADM_ID, index_file, offset, 330712483);

    *r_size = offset_size;
    *result = (struct Feature *)malloc(offset_size * sizeof(struct Feature));
    memset(*result, 0, offset_size * sizeof(struct Feature));

    // 开始读取
    int i = 0;
    char buffer[1024];
    char read_buffer[64];
    int c_subject_id = -1;
    for (i = 0; i < offset_size; i++)
    {
        fseeko64(csv_file, offset[i], SEEK_SET);
        fgets(buffer, 1024, csv_file);

        unsigned int index = 0;
        buff_index_move(buffer, &index, 1);
        str_cpy(buffer, read_buffer, &index, ',');
        c_subject_id = my_atoi(read_buffer);
        if (c_subject_id != SUBJECT_ID)
            continue;
        index += 1;

        buff_index_move(buffer, &index, 2);

        str_cpy(buffer, read_buffer, &index, ',');
        (*result)[i].ITEMID = my_atoi(read_buffer);
        index += 1;

        str_cpy(buffer, read_buffer, &index, ',');
        (*result)[i].time = str_2_time_stamp(read_buffer);
        index += 1;

        buff_index_move(buffer, &index, 2);

        str_cpy_2(buffer, read_buffer, &index, ',', '\"');
        (*result)[i].value = my_atof(read_buffer);
    }
    free(offset);
}

void filter_by_itemid(struct Feature *dst, int *index, struct Feature *src, int src_size, struct Feature_ID *features, int feature_size, struct StaticInformation *sInfor)
{
    int i = 0, j = 0;
    for (i = 0; i < src_size; i++)
    {
        // 过滤的同时顺便更新起止时间
        if (sInfor->begintime > src[i].time || sInfor->begintime == 0)
            sInfor->begintime = src[i].time;
        if (sInfor->endtime < src[i].time || sInfor->endtime == 0)
            sInfor->endtime = src[i].time;
        // 过滤
        // TODO:可以将feature排序再筛选
        for (j = 0; j < feature_size; j++)
            if (is_in_array(src[i].ITEMID, features[j].ITEMID))
            {
                dst[*index].ITEMID = src[i].ITEMID;
                dst[*index].time = src[i].time;
                dst[*index].value = src[i].value;
                (*index)++;
                break;
            }
    }
}

int feature_cmp(const void *a, const void *b)
{
    struct Feature *pa = (struct Feature *)a;
    struct Feature *pb = (struct Feature *)b;
    long long ta = pa->time;
    long long tb = pb->time;
    return (int)(ta - tb);
}

void feature_extract(int HADMID, int SUBJECT_ID, int *r_size, struct Feature **result, struct Feature_ID *features, int feature_size, struct StaticInformation *sInfor)
{
    // FILE *LABEVENTS_index = fopen(LABEVENTS_INDEX, "rb");
    FILE *CHARTEVENTS_index = fopen(CHARTEVENTS_INDEX, "rb");
    FILE *LABEVENTS_csv = fopen(LABEVENTS, "r");
    FILE *CHARTEVENTS_csv = fopen(CHARTEVENTS, "r");

    struct Feature *labevents;
    int labevents_size = 0;
    LABEVENTS_read(LABEVENTS_csv, HADMID, &labevents, &labevents_size);

    struct Feature *chartevents;
    int chartevents_size = 0;
    CHARTEVENTS_read(CHARTEVENTS_index, HADMID, SUBJECT_ID, CHARTEVENTS_csv, &chartevents, &chartevents_size);

    *result = (struct Feature *)malloc((labevents_size + chartevents_size) * sizeof(struct Feature));
    int result_index = 0;
    // 遍历选出的所有特征数据, 根据要求的特征ID进行筛选
    filter_by_itemid(*result, &result_index, labevents, labevents_size, features, feature_size, sInfor);
    filter_by_itemid(*result, &result_index, chartevents, chartevents_size, features, feature_size, sInfor);
    *r_size = result_index;

    // 排序
    qsort(*result, result_index, sizeof(struct Feature), feature_cmp);

    free(chartevents);
    free(labevents);

    // fclose(LABEVENTS_index);
    fclose(CHARTEVENTS_index);
    fclose(LABEVENTS_csv);
    fclose(CHARTEVENTS_csv);
}