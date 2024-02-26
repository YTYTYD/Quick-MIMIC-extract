#include "fextract.h"
/**
 * 复制字符串并将字符串指针向后移动对应位
 */
void m_strcpy(char **output_buffer, const char *src, int src_len)
{
    strcpy(*output_buffer, src);
    (*output_buffer) += src_len;
}

/**
 * sprintf后移动指针到字符串末尾
 */
void m_sprintf(char **buffer, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf((*buffer), fmt, args);
    va_end(args);
    (*buffer) += strlen(*buffer);
}

void table_head_output(struct task e_task, char **output_buffer)
{
    int i;
    for (i = 0; i < e_task.feature_ids_size; i++)
        m_sprintf(output_buffer, "%s,", e_task.feature_ids[i].name);
    if (e_task.age)
        m_strcpy(output_buffer, "Age,", 4);
    if (e_task.gender)
        m_strcpy(output_buffer, "Gender,", 4);
    if (e_task.Unit1)
        m_strcpy(output_buffer, "Unit1,", 6);
    if (e_task.Unit2)
        m_strcpy(output_buffer, "Unit2,", 6);
    if (e_task.HospAdmTime)
        m_strcpy(output_buffer, "HospAdmTime,", 12);
    if (e_task.ICULOS)
        m_strcpy(output_buffer, "ICULOS,", 7);
    if (e_task.drug)
        m_strcpy(output_buffer, "Strategy,GSN,NDC,", 17);
#ifdef MIMICIII
    if (e_task.note)
        m_strcpy(output_buffer, "TEXT,", 5);
#endif
    m_strcpy(output_buffer, "\n", 1);
}

void static_info_output(struct task e_task, struct StaticInformation sInfor, char **output_buffer, int iculos)
{
    if (e_task.age)
        m_sprintf(output_buffer, "%d,", sInfor.Age);
    if (e_task.gender)
        m_sprintf(output_buffer, "%d,", sInfor.Gender);
    if (e_task.Unit1)
        m_sprintf(output_buffer, "%d,", sInfor.Unit1);
    if (e_task.Unit2)
        m_sprintf(output_buffer, "%d,", sInfor.Unit2);
    if (e_task.HospAdmTime)
        m_sprintf(output_buffer, "%lld,", sInfor.HospAdmiTime);
    if (e_task.ICULOS)
        m_sprintf(output_buffer, "%d,", iculos);
}

/**
 * 根据两个ID进行提取
 */
void extract(int HADM_ID, int SUBJECT_ID, struct task e_task, char **ext_str_result)
{
    // 病人基本信息
    struct StaticInformation sInfor;
    sInfor.begintime = 0;
    sInfor.endtime = 0;
    sInfo_extract(&sInfor, HADM_ID, SUBJECT_ID);
#ifdef MIMICIII
    // 医嘱信息
    struct NOTE *notes;
    int notes_size = 0;
    if (e_task.note)
    {
        note_extract(HADM_ID, &notes, &notes_size);
    }
#endif
    // 药物信息
    struct DRUG *drugs = NULL;
    int drugs_size = 0;
    if (e_task.drug)
    {
        drug_extract(HADM_ID, &drugs, &drugs_size, &sInfor);
    }
    // 化验信息
    struct Feature *features;
    int features_size = 0;
    feature_extract(HADM_ID, SUBJECT_ID, &features_size, &features, e_task.feature_ids, e_task.feature_ids_size, &sInfor);

    // 确定开始时间, 此时sInfor.HospAdmiTime中存储的是入院时间
    switch (e_task.start_time)
    {
    case 0:
        break;
    case 1:
        sInfor.begintime = sInfor.HospAdmiTime;
        break;
    case 2:
        if (sInfor.HospAdmiTime < sInfor.begintime)
        {
            sInfor.begintime = sInfor.HospAdmiTime;
        }
        break;
    default:
        break;
    }
    // 计算住院时长时长
    sInfor.HospAdmiTime = sInfor.endtime / 3600 - sInfor.begintime / 3600;

    int i = 0, j = 0, k = 0;
    char *result_buffer = malloc(CSV_STR_MAX_LEN);
    memset(result_buffer, 0, CSV_STR_MAX_LEN);
    char *result_buffer_ptr = result_buffer;
    // 输出表头
    table_head_output(e_task, &result_buffer_ptr);

    // 从开始时间到结束时间遍历
    int drug_index = 0, note_index = 0, feature_index = 0;
    for (i = 0; i < sInfor.HospAdmiTime; i++)
    {
        long long ctime = sInfor.begintime / 3600 + (long long)i; // 当前时间(小时)
        //----------------------------------------------------------------------------------------
        // 遍历给定的特征ID表

        // 暂存每个特征数据查询到的数据的总和, 用于求均值
        double *feature_data = (double *)malloc(e_task.feature_ids_size * sizeof(double));
        int *feature_data_counter = (int *)malloc(e_task.feature_ids_size * sizeof(int));
        memset(feature_data, 0, e_task.feature_ids_size * sizeof(double));
        memset(feature_data_counter, 0, e_task.feature_ids_size * sizeof(int));
        for (j = 0; j < e_task.feature_ids_size; j++)
        {
            // 遍历当前时间点的特征数据, 找到匹配的第一个数据
            for (k = feature_index; k < features_size && features[k].time / 3600 <= ctime; k++)
            {
                // 判断当前数据是否是指定itemID(有多个对应ID)的数据
                if (is_in_array(features[k].ITEMID, e_task.feature_ids[j].ITEMID) && features[k].time / 3600 == ctime)
                {
                    feature_data[j] += features[k].value;
                    feature_data_counter[j] += 1;
                }
            }
        }
        // 输出
        for (j = 0; j < e_task.feature_ids_size; j++)
        {
            if (feature_data_counter[j] != 0)
                m_sprintf(&result_buffer_ptr, "%f", feature_data[j] / feature_data_counter[j]);
            m_strcpy(&result_buffer_ptr, ",", 1);
        }
        free(feature_data);
        free(feature_data_counter);
        //----------------------------------------------------------------------------------------
        // 移动指针到下一个时间点的数据
        while (feature_index < features_size && features[feature_index].time / 3600 <= ctime)
            feature_index++;
        // 输出病人基本信息
        static_info_output(e_task, sInfor, &result_buffer_ptr, i + 1);
        // 输出药品信息
        if (e_task.drug)
        {
            char *Strategy = malloc(1024 * 1024);
            Strategy[0] = '\0';
            char *GSN = malloc(6 * 1000);
            GSN[0] = '\0';
            char *NDC = malloc(11 * 1000);
            NDC[0] = '\0';
            char trans_buff[32];
            for (drug_index = 0; drug_index < drugs_size; drug_index++)
            {
                if (drugs[drug_index].STARTDATE / 3600 <= ctime && drugs[drug_index].ENDDATE / 3600 >= ctime)
                {
                    if (strlen(Strategy) != 0)
                    {
                        strcat(Strategy, ",");
                        strcat(GSN, ",");
                        strcat(NDC, ",");
                    }
                    strcat(Strategy, drugs[drug_index].PROD_STRENGTH);
                    my_itoa(drugs[drug_index].GSN, trans_buff);
                    strcat(GSN, trans_buff);
                    sprintf(trans_buff, "%lld", drugs[drug_index].NDC);
                    strcat(NDC, trans_buff);
                }
            }
            m_sprintf(&result_buffer_ptr, "\"%s\",\"%s\",\"%s\",", Strategy, GSN, NDC);
            free(Strategy);
            free(GSN);
            free(NDC);
        }

#ifdef MIMICIII
        if (e_task.note)
        {
            // 输出医嘱信息
            if (notes[note_index].DATE / 3600 == ctime)
            {
                m_sprintf(&result_buffer_ptr, "\"%s\",", notes[note_index].TEXT);
                if (note_index < notes_size)
                    note_index++;
            }
        }
#endif
        m_strcpy(&result_buffer_ptr, "\n", 1);

        if ((result_buffer_ptr - result_buffer) > CSV_STR_MAX_LEN * 0.9)
        {   //丢弃超出缓冲区容量90%的数据, 视为异常数据
            (*ext_str_result) = malloc(strlen(result_buffer) + 1);
            strcpy(*ext_str_result, "error");
            goto EXTEND;
        }
    }
    (*ext_str_result) = malloc(strlen(result_buffer) + 1);
    strcpy(*ext_str_result, result_buffer);
EXTEND:
    free(result_buffer);

    // 释放
#ifdef MIMICIII
    if (e_task.note)
    {
        // 释放医嘱文本内存
        for (i = 0; i < notes_size; i++)
            free(notes[i].TEXT);
        free(notes);
    }
#endif
    if (e_task.drug)
    {
        if (drugs != NULL)
            free(drugs);
    }
    free(features);
}
