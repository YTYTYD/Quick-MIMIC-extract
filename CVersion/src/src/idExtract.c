#include "idExtract.h"

const int MAX_ICD_CODE_NUM = 100;

int binary_search(const char *arr[], int low, int high, char *target)
{
    while (low <= high)
    {
        int mid = (high + low) / 2;
        int comparison = strcmp(arr[mid], target);

        if (comparison == 0)
        {
            return mid;
        }
        else if (comparison < 0)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return -1;
}

/**
 * 从一行数据中分离出HADM_ID和SUBJECT_ID
 */
void get_HID_SID(char *buff, int *HID, int *SID, int *ICD_CODE)
{
    unsigned int str_index = 0;
#ifdef MIMICIII
    buff_index_move(buff, &str_index, 1);
#endif
    *SID = my_atoi(&buff[str_index]);
    buff_index_move(buff, &str_index, 1);

    *HID = my_atoi(&buff[str_index]);
    buff_index_move(buff, &str_index, 2);
    *ICD_CODE = -1;
#ifdef MIMICIII
    if (buff[str_index] == '\0')
        return;
    str_index += 1;
#endif
    char icd_code_str[10];
#ifdef MIMICIII
    str_cpy(buff, icd_code_str, &str_index, '"');
#elif defined(MIMICIV)
    str_cpy(buff, icd_code_str, &str_index, ',');
#endif
    *ICD_CODE = binary_search(icd_codes, 0, icd_code_nums, icd_code_str);
}

/**
 * 判断给定HID是否在列表中
 */
int is_id_in_list(struct ID_node *list, int list_size, int HADM_ID)
{
    int start = 0;
    int end = list_size;
    while (start < end)
    {
        int mid = (start + end) / 2;
        if (list[mid].HADM_ID == HADM_ID)
            return 1; // 存在
        else if (list[mid].HADM_ID > HADM_ID)
            end = mid;
        else
            start = mid + 1;
    }
    return 0; // 不存在
}

int id_cmp(const void *a, const void *b)
{
    struct ID_node *n_a = (struct ID_node *)a;
    struct ID_node *n_b = (struct ID_node *)b;
    return n_a->HADM_ID - n_b->HADM_ID;
}

void init_ID_node(struct ID_node *node, int SID, int HID, int ICD_CODE)
{
    node->SUBJECT_ID = SID;
    node->HADM_ID = HID;
    node->ICD_CODE = (int *)malloc(MAX_ICD_CODE_NUM * sizeof(int));
    node->ICD_CODE[0] = ICD_CODE;
    node->ICD_CODE_NUM = 1;
}

void id_extract(struct ID_node **result, int *r_size, int MPI_rank, int MPI_size)
{
    // 若ID为0, 则提取数据, 打乱, 分发给其它进程
    if (MPI_rank == 0)
    {
        struct H_ID_node
        {
            struct ID_node data;
            struct H_ID_node *left;
            struct H_ID_node *right;
        };
        FILE *fp = NULL;
        char buff[255]; // 文件读取buff
        // 分配内存, 初始化
        struct H_ID_node *HADMID = (struct H_ID_node *)malloc(sizeof(struct H_ID_node) * ID_LEN);
        int HADMID_size = 0;                                  // 标记当前存储的数据量
        memset(HADMID, 0, sizeof(struct H_ID_node) * ID_LEN); // 全部初始化为0
        // 读诊断信息表, 查病案号和患者编号
        fp = fopen(DIAGNOSES_ICD, "r");

        fscanf(fp, "%s", buff); // 读取第一行表头
        // 此处用二叉树, 方便判断ID是否已存在
        while (fscanf(fp, "%s", buff) != -1)
        {
            int SID = 0;
            int HID = 0;
            int ICD_CODE = 0;
            // 提取HADM_ID和SUBJECT_ID
            get_HID_SID(buff, &HID, &SID, &ICD_CODE);

            // 向树中插入HID
            if (HADMID_size == 0)
            {
                init_ID_node(&HADMID[0].data, SID, HID, ICD_CODE);
                HADMID_size++;
            }
            else
            {
                struct H_ID_node *H_ID_ptr = &HADMID[0];
                // 找到树中对应的叶子节点并插入
                while (1)
                {
                    if (H_ID_ptr->data.HADM_ID > HID)
                    {
                        if (H_ID_ptr->left)
                            H_ID_ptr = H_ID_ptr->left;
                        else
                        {
                            H_ID_ptr->left = &HADMID[HADMID_size];
                            init_ID_node(&HADMID[HADMID_size].data, SID, HID, ICD_CODE);
                            HADMID_size++;
                            H_ID_ptr = H_ID_ptr->left;
                            break;
                        }
                    }
                    else if (H_ID_ptr->data.HADM_ID < HID)
                    {
                        if (H_ID_ptr->right)
                            H_ID_ptr = H_ID_ptr->right;
                        else
                        {
                            H_ID_ptr->right = &HADMID[HADMID_size];
                            init_ID_node(&HADMID[HADMID_size].data, SID, HID, ICD_CODE);
                            HADMID_size++;
                            H_ID_ptr = H_ID_ptr->right;
                            break;
                        }
                    }
                    else
                    { // 相等, 已存在
                        H_ID_ptr->data.ICD_CODE[H_ID_ptr->data.ICD_CODE_NUM] = ICD_CODE;
                        H_ID_ptr->data.ICD_CODE_NUM++;
                        break;
                    }
                }
            }
        }

        fclose(fp);

        int i = 0, j = 0;
        // 打乱
        struct H_ID_node temp;
        struct timeval temp_time;
        gettimeofday(&temp_time, NULL);
        srand(temp_time.tv_usec);
        for (i = 0; i < HADMID_size; i++)
        {
            int target_index = rand() % HADMID_size;
            // 交换
            temp.data = HADMID[i].data;
            HADMID[i].data = HADMID[target_index].data;
            HADMID[target_index].data = temp.data;
        }
        // 发送
        int size_per_process = HADMID_size / (MPI_size - 1);
        // 遍历, 给每个进程发送数据
        for (i = 1; i < MPI_size; i++)
        {
            int true_size = size_per_process;
            int start_index = (i - 1) * size_per_process;
            if (start_index + size_per_process > HADMID_size || HADMID_size - (start_index + size_per_process) < size_per_process)
            {
                true_size = HADMID_size - start_index;
            }
            // 发送数量
            MPI_Send(&true_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            // 发送HID
            int *id_buff = (int *)malloc(true_size * sizeof(int));
            for (j = 0; j < true_size; j++)
            {
                id_buff[j] = HADMID[start_index + j].data.HADM_ID;
            }
            MPI_Send(id_buff, true_size, MPI_INT, i, 0, MPI_COMM_WORLD);
            // 发送SID
            for (j = 0; j < true_size; j++)
            {
                id_buff[j] = HADMID[start_index + j].data.SUBJECT_ID;
            }
            MPI_Send(id_buff, true_size, MPI_INT, i, 0, MPI_COMM_WORLD);
            // 发送ICD_CODE
            for (j = 0; j < true_size; j++)
            {
                MPI_Send(&HADMID[start_index + j].data.ICD_CODE_NUM, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(HADMID[start_index + j].data.ICD_CODE, HADMID[start_index + j].data.ICD_CODE_NUM, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            free(id_buff);
        }
        for (i = 0; i < HADMID_size; i++)
        {
            free(HADMID[i].data.ICD_CODE);
        }
        free(HADMID);
    }
    else
    {
        // 接收ID数量
        MPI_Recv(r_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        *result = (struct ID_node *)malloc((*r_size) * sizeof(struct ID_node));
        // 接收数据缓冲区
        int *IDs = (int *)malloc((*r_size) * sizeof(int));
        // 接收HID
        MPI_Recv(IDs, *r_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int j = 0;
        for (j = 0; j < *r_size; j++)
            (*result)[j].HADM_ID = IDs[j];
        // 接收SID
        MPI_Recv(IDs, *r_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (j = 0; j < *r_size; j++)
            (*result)[j].SUBJECT_ID = IDs[j];
        // 接受ICD_CODE
        for (j = 0; j < *r_size; j++)
        {
            MPI_Recv(&(*result)[j].ICD_CODE_NUM, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            (*result)[j].ICD_CODE = (int *)malloc((*result)[j].ICD_CODE_NUM * sizeof(int));
            MPI_Recv((*result)[j].ICD_CODE, (*result)[j].ICD_CODE_NUM, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        free(IDs);
    }

    // 按HADM_ID排序, 方便后续查找
    qsort(*result, *r_size, sizeof(struct ID_node), id_cmp);
}

int icd_search(int *list, int start, int end, int icd_code)
{
    int mid;
    while (start < end)
    {
        mid = (start + end) / 2;
        if (list[mid] == icd_code)
            return 1;
        if (list[mid] > icd_code)
            end = mid;
        else
            start = mid + 1;
    }
    return 0;
}

int is_ICD_in_list(struct ID_node node, int *task_ICD_list, int task_ICD_list_size)
{
    for (int i = 0; i < node.ICD_CODE_NUM; i++)
        if (icd_search(task_ICD_list, 0, task_ICD_list_size, node.ICD_CODE[i]))
            return 1;
    return 0;
}