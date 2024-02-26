#include <utils.h>

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
        while (buff[*index] != ',' && buff[*index] != '\0')
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

/**
 * 判断一个数n是否在数组arr中
 * 其中arr无数据的部分要填充-1
 * 存在返回1, 不存在返回0
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

int time_diff_ms(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) * 1000 + (int)((end.tv_usec - start.tv_usec) / 1000);
}

/**
 * 测试用函数, 返回当前程序占用的内存, 单位KB
 */
int get_current_mem()
{
    int pid = getpid();
    char file_name[64];
    sprintf(file_name, "/proc/%d/status", pid);
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
        return -1;

    int mem_size = 0, i;
    char read_buffer[512], name[128];
    while (fgets(read_buffer, 512, file) != NULL)
    {
        if (strstr(read_buffer, "VmRSS:") != NULL)
        {
            sscanf(read_buffer, "%s %d", name, &mem_size);
            break;
        }
    }
    fclose(file);
    return mem_size;
}