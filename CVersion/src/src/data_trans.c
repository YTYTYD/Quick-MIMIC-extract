#include "data_trans.h"

int p_recv_data(int MPI_rank, int connfd, int stop_flag)
{
    int recv_len = 0;
    char *recv_buffer = malloc(CSV_STR_MAX_LEN);
    int id = 0;
    while (1)
    {
        // 向进程发送开始发送信号
        MPI_Send(&stop_flag, 1, MPI_INT, MPI_rank, 0, MPI_COMM_WORLD);
        // 接收进程发送的数据长度, 收到0时退出循环
        MPI_Recv(&recv_len, 1, MPI_INT, MPI_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (recv_len == 0)
            break;
        // 接收病案号ID
        MPI_Recv(&id, 1, MPI_INT, MPI_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // 接收进程发送的数据
        MPI_Recv(recv_buffer, recv_len, MPI_CHAR, MPI_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // 通过socket向外发送数据
        // 发送长度
        if (send(connfd, &recv_len, 4, 0) <= 0)
            goto ERREND;
        // 发送病案号ID
        if (send(connfd, &id, 4, 0) <= 0)
            goto ERREND;
        // 发送数据
        if (send(connfd, recv_buffer, recv_len, 0) <= 0)
            goto ERREND;
        else
            continue;
    ERREND:
        stop_flag = 2;
        break;
    }
    free(recv_buffer);
    return stop_flag;
}

void p_send_data(char **result, int *h_ids, int result_size)
{
    int i, next_action_flag = 0, str_len = 0;
    for (i = 0; i < result_size; i++)
    {
        if (strcmp(result[i], "error") == 0)
            continue;
        // 接收发送信号, 判断继续发送还是终止
        MPI_Recv(&next_action_flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (next_action_flag == 2)
            break;
        // 发送长度
        str_len = strlen(result[i]);
        MPI_Send(&str_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // 发送ID
        MPI_Send(&h_ids[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // 发送数据
        MPI_Send(result[i], str_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    // 接收最后一个信号
    MPI_Recv(&next_action_flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // 发送长度0, 表示结束
    str_len = 0;
    MPI_Send(&str_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}