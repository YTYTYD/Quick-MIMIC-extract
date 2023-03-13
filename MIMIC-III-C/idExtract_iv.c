/**
 * 从一行数据中分离出HADM_ID和SUBJECT_ID
 */
void get_HID_SID(char *buff, int *HID, int *SID)
{
    int str_index = 0;
    // while (buff[str_index] != ',')
    //     str_index++;
    // str_index++;
    while (buff[str_index] != ',')
    {
        *SID = (*SID * 10 + (buff[str_index] - '0'));
        str_index++;
    }
    str_index++;
    while (buff[str_index] != ',')
    {
        *HID = (*HID * 10 + (buff[str_index] - '0'));
        str_index++;
    }
}

void id_extract(struct H_ID_node **result, int *r_size)
{
    FILE *fp = NULL;
    char buff[255]; // 文件读取buff
    const int MAXSIZE = 651048;
    // 分配内存, 初始化
    *result = (struct H_ID_node *)malloc(sizeof(struct H_ID_node) * MAXSIZE);
    struct H_ID_node *HADMID = *result;
    int HADMID_size = 0;                                   // 标记当前存储的数据量
    memset(HADMID, 0, sizeof(struct H_ID_node) * MAXSIZE); // 全部初始化为0

    // 读诊断信息表, 查病案号和患者编号
    fp = fopen(DIAGNOSES_ICD, "r");

    int counter = 0;
    fscanf(fp, "%s", buff); // 读取第一行表头
    while (1)
    {
        int r = fscanf(fp, "%s", buff);
        if (r == -1)
            break;
        int SID = 0;
        int HID = 0;
        // 提取HADM_ID和SUBJECT_ID
        get_HID_SID(buff, &HID, &SID);

        // 向树中插入HID
        if (HADMID_size == 0)
        {
            HADMID[0].SUBJECT_ID = SID;
            HADMID[0].HADM_ID = HID;
            HADMID_size++;
        }
        else
        {
            struct H_ID_node *H_ID_ptr = &HADMID[0];
            // 找到树中对应的叶子节点并插入
            while (1)
            {
                if (H_ID_ptr->HADM_ID > HID)
                {
                    if (H_ID_ptr->left)
                        H_ID_ptr = H_ID_ptr->left;
                    else
                    {
                        H_ID_ptr->left = &HADMID[HADMID_size];
                        HADMID[HADMID_size].HADM_ID = HID;
                        HADMID[HADMID_size].SUBJECT_ID = SID;
                        HADMID_size++;
                        H_ID_ptr = H_ID_ptr->left;
                        break;
                    }
                }
                else if (H_ID_ptr->HADM_ID < HID)
                {
                    if (H_ID_ptr->right)
                        H_ID_ptr = H_ID_ptr->right;
                    else
                    {
                        H_ID_ptr->right = &HADMID[HADMID_size];
                        HADMID[HADMID_size].HADM_ID = HID;
                        HADMID[HADMID_size].SUBJECT_ID = SID;
                        HADMID_size++;
                        H_ID_ptr = H_ID_ptr->right;
                        break;
                    }
                }
                else // 相等, 已存在, 退出
                    break;
            }
        }
        counter += 1;
    }
    *r_size = HADMID_size;

    fclose(fp);
}