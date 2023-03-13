int my_atoi(char *str)
{
    int i = 0;
    int result = 0;
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result;
}

double my_atof(char *str)
{
    double result = 0;
    result = (double)my_atoi(str);
    int index = 0;
    while (str[index] != '.')
    {
        if (str[index] < '0' || str[index] > '9')
            return result;
        index++;
    }
    int i = index + 1;
    while (str[i] >= '0' && str[i] <= '9')
    {
        result += (str[i] - '0') / pow(10, i - index);
        i++;
    }
    return result;
}

void my_itoa(int n, char *result)
{
    sprintf(result, "%d", n);
}

long long str_2_time_stamp(char *str_time)
{
    struct tm stm;
    int iY, iM, iD, iH, iMin, iS;

    memset(&stm, 0, sizeof(stm));

    iY = my_atoi(str_time);
    iM = my_atoi(str_time + 5);
    iD = my_atoi(str_time + 8);
    iH = my_atoi(str_time + 11);
    iMin = my_atoi(str_time + 14);
    iS = my_atoi(str_time + 17);

    stm.tm_year = iY - 1900;
    stm.tm_mon = iM - 1;
    stm.tm_mday = iD;
    stm.tm_hour = iH;
    stm.tm_min = iMin;
    stm.tm_sec = iS;

    return mktime(&stm);
}

void buff_index_move(char *buff, unsigned int *index, int n)
{
    int i = 0;
    for (i = 0; i < n; i++)
    {
        while (buff[*index] != ',')
            (*index)++;
        (*index)++;
    }
}

void str_cpy(char *buff, char *dst, unsigned int *start, char separator)
{
    int len = 0;
    for (*start;; (*start)++)
    {
        if (buff[*start] == separator)
        {
            dst[len] = '\0';
            return;
        }
        dst[len] = buff[*start];
        len++;
    }
}

void str_cpy_2(char *buff, char *dst, unsigned int *start, char separator, char no_copy_char)
{
    int len = 0;
    for (*start;; (*start)++)
    {
        if (buff[*start] == separator)
        {
            dst[len] = '\0';
            (*start)++;
            return;
        }
        if (buff[*start] != no_copy_char)
        {
            dst[len] = buff[*start];
            len++;
        }
    }
}

unsigned int get_all_offset(int hid, FILE *index_file, unsigned int *result, unsigned int max)
{
    char buffer[8];

    /*二分法找到当前HID对应的位置*/
    unsigned int start = 0, end = max - 1;
    unsigned int current = (start + end) / 2; // 当前指针位置
    unsigned int chid = 0;                    // 当前读取到的hid
    while (1)
    {
        fseek(index_file, (current * 8), SEEK_SET);
        /*读取8字节数据*/
        fread(buffer, 8, 1, index_file);
        chid = (buffer[0] & 0xff) + ((buffer[1] & 0xff) << 8) + ((buffer[2] & 0xff) << 16) + ((buffer[3] & 0xff) << 24);
        if (chid != hid)
        {
            if (current == start)
                return 0; // 未找到
            if (chid > hid)
                end = current;
            else // chid < hid
                start = current;
            current = (start + end) / 2;
        }
        else // 找到
            break;
    }

    /*左右查找出所有的偏移*/
    unsigned int counter = 0;
    unsigned int offset = 0;
    unsigned int index = current;
    while (index >= 0)
    { // 向前查找
        fseek(index_file, index * 8, SEEK_SET);
        /*读取8字节数据*/
        fread(buffer, 8, 1, index_file);
        chid = (buffer[0] & 0xff) + ((buffer[1] & 0xff) << 8) + ((buffer[2] & 0xff) << 16) + ((buffer[3] & 0xff) << 24);
        offset = (buffer[4] & 0xff) + ((buffer[5] & 0xff) << 8) + ((buffer[6] & 0xff) << 16) + ((buffer[7] & 0xff) << 24);
        if (chid != hid)
            break;
        result[counter] = offset;
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
        // XXX:代码重复, 有待改进
        fseek(index_file, index * 8, SEEK_SET);
        fread(buffer, 8, 1, index_file);
        chid = (buffer[0] & 0xff) + ((buffer[1] & 0xff) << 8) + ((buffer[2] & 0xff) << 16) + ((buffer[3] & 0xff) << 24);
        offset = (buffer[4] & 0xff) + ((buffer[5] & 0xff) << 8) + ((buffer[6] & 0xff) << 16) + ((buffer[7] & 0xff) << 24);
        if (chid != hid)
            break;
        result[counter] = offset;
        index += 1;
        counter += 1;
    }
    return counter;
}

unsigned int get_all_offset_64(int hid, FILE *index_file, long long *result, unsigned int max)
{
    char buffer[9];

    /*二分法找到当前HID对应的位置*/
    long long start = 0, end = max - 1;
    long long current = (start + end) / 2; // 当前指针位置
    unsigned int chid = 0;                 // 当前读取到的hid
    while (1)
    {
        fseeko64(index_file, (current * 9), SEEK_SET);
        /*读取9字节数据*/
        fread(buffer, 9, 1, index_file);
        chid = (buffer[0] & 0xff) + ((buffer[1] & 0xff) << 8) + ((buffer[2] & 0xff) << 16) + ((buffer[3] & 0xff) << 24);
        if (chid != hid)
        {
            if (current == start)
                return 0; // 未找到
            if (chid > hid)
                end = current;
            else // chid < hid
                start = current;
            current = (start + end) / 2;
        }
        else // 找到
            break;
    }

    /*左右查找出所有的偏移*/
    unsigned int counter = 0;
    long long offset = 0;
    long long index = current;
    while (index >= 0)
    { // 向前查找
        fseeko64(index_file, index * 9, SEEK_SET);
        /*读取9字节数据*/
        fread(buffer, 9, 1, index_file);
        chid = (buffer[0] & 0xff) + ((buffer[1] & 0xff) << 8) + ((buffer[2] & 0xff) << 16) + ((buffer[3] & 0xff) << 24);
        offset = (long long)(buffer[4] & 0xff) + ((long long)(buffer[5] & 0xff) << 8) + ((long long)(buffer[6] & 0xff) << 16) + ((long long)(buffer[7] & 0xff) << 24) + ((long long)(buffer[8] & 0xff) << 32);
        if (chid != hid)
            break;
        result[counter] = offset;
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
        // XXX:代码重复, 有待改进
        fseeko64(index_file, index * 9, SEEK_SET);
        fread(buffer, 9, 1, index_file);
        chid = (buffer[0] & 0xff) + ((buffer[1] & 0xff) << 8) + ((buffer[2] & 0xff) << 16) + ((buffer[3] & 0xff) << 24);
        offset = (long long)(buffer[4] & 0xff) + ((long long)(buffer[5] & 0xff) << 8) + ((long long)(buffer[6] & 0xff) << 16) + ((long long)(buffer[7] & 0xff) << 24) + ((long long)(buffer[8] & 0xff) << 32);
        if (chid != hid)
            break;
        result[counter] = offset;
        index += 1;
        counter += 1;
    }
    return counter;
}

/**
 * 判断一个数n是否在数组arr中
 * 其中arr无数据的要填充-1
 */
int is_in_array(int n, int *arr)
{
    int j = 0;
    while (arr[j] != -1)
    {
        if (arr[j] == n)
            return 1;
        j++;
    }
    return 0;
}

#ifdef __linux
int time_diff_ms(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) * 1000 + (int)((end.tv_usec - start.tv_usec) / 1000);
}
#endif
