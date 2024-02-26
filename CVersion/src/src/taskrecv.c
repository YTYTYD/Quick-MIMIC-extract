#include "taskrecv.h"

int listenfd, connfd;
struct sockaddr_in servaddr;
unsigned char *recv_buffer;
const int NET_RECV_MAX_LEN = 4096;

int network_init()
{
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket create fail\n");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9812); // 默认端口为9812

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("bind fail\n");
        return -1;
    }

    if (listen(listenfd, 10) == -1)
    {
        printf("listen fail\n");
        return -1;
    }
    // 分配接收缓存
    recv_buffer = (unsigned char *)malloc(NET_RECV_MAX_LEN);
    return 0;
}

int buff_to_int(unsigned char *buff)
{
    // buffer中内容转int, 小端法存储
    return ((unsigned)buff[3] << 24) + ((unsigned)buff[2] << 16) + ((unsigned)buff[1] << 8) + (unsigned)buff[0];
}

struct task data_unpack(unsigned char *buffer, int buffer_size)
{
    struct task new_task;
    new_task.feature_ids_size = buff_to_int(buffer);
    new_task.ICD_list_size = buff_to_int(buffer + 4);
    new_task.age = buff_to_int(buffer + 8);
    new_task.gender = buff_to_int(buffer + 12);
    new_task.Unit1 = buff_to_int(buffer + 16);
    new_task.Unit2 = buff_to_int(buffer + 20);
    new_task.HospAdmTime = buff_to_int(buffer + 24);
    new_task.ICULOS = buff_to_int(buffer + 28);
    new_task.drug = buff_to_int(buffer + 32);
    new_task.note = buff_to_int(buffer + 36);
    new_task.task_id = buff_to_int(buffer + 40);
    new_task.start_time = buff_to_int(buffer + 44);
    int i, j, buffer_index = 48;
    new_task.ICD_list = (int *)malloc((new_task.ICD_list_size + 1) * sizeof(int));
    memset(new_task.ICD_list, -1, (new_task.ICD_list_size + 1) * sizeof(int));
    for (i = 0; i < new_task.ICD_list_size; i++)
    {
        new_task.ICD_list[i] = buff_to_int(buffer + buffer_index);
        buffer_index += 4;
    }
    new_task.feature_ids = (struct Feature_ID *)malloc(new_task.feature_ids_size * sizeof(struct Feature_ID));
    memset(new_task.feature_ids, -1, new_task.feature_ids_size * sizeof(struct Feature_ID));
    for (i = 0; i < new_task.feature_ids_size; i++)
    {
        // 获取当前特征的特征数量
        int item_id_size = buff_to_int(buffer + buffer_index);
        buffer_index += 4;
        // 复制特征名
        strcpy(new_task.feature_ids[i].name, (const char *)buffer + buffer_index);
        buffer_index += (strlen((const char *)buffer + buffer_index) + 1);
        new_task.feature_ids[i].name;
        // 复制每个ITEM ID
        for (j = 0; j < item_id_size; j++)
        {
            new_task.feature_ids[i].ITEMID[j] = buff_to_int(buffer + buffer_index);
            buffer_index += 4;
        }
    }
    return new_task;
}