#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mpi.h"
#include <sys/time.h>

#include "defines.h"
#include "taskrecv.h"
#include "consts.h"
#include "globals.h"
#include "utils.h"
#include "init.h"
#include "taskInit.h"
#include "idExtract.h"
#include "admissionsPRead.h"
#include "data_trans.h"

// ID列表及数量
struct ID_node *HADM_IDs;
int HADM_IDs_size = 0;
int continue_signal = 0;
int stop_signal = -1;

#include "drugRead.h"
#include "drugExtract.h"
#include "eventsRead.h"
#include "featureExtract.h"
#include "patientsPRead.h"
#include "icustaysExtractPRead.h"
#include "transfersPRead.h"
#include "noteRead.h"
#include "noteExtract.h"
#include "sInfoExtract.h"

#include "fextract.h"

int main()
{
    int MPI_rank, MPI_size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);

    if (init(MPI_size) != 0)
    {
        MPI_Finalize();
        return 0;
    }
    int abort_flag = 0;
    if (MPI_rank == 0)
    {
        // 主进程监听端口
        if (network_init() != 0)
            abort_flag = 1;
    }
    MPI_Bcast(&abort_flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (abort_flag == 1)
    {
        // 端口监听异常, 中止
        MPI_Finalize();
        return 0;
    }

    // DEBUG 日志输出
    char log_file_path[512];
    strcpy(log_file_path, OUTPUT_DIR);
    strcat(log_file_path, "log/");
    if (access(log_file_path, 0) == -1)
    {
        mkdir(log_file_path, S_IRWXU);
    }
    my_itoa(MPI_rank, log_file_path + strlen(log_file_path));
    strcat(log_file_path, ".log");
    FILE *log_file = fopen(log_file_path, "w");
    //----------------------------------------------------------------------------------------

#ifdef TEST1
    // 计时
    struct timeval begintime;
    gettimeofday(&begintime, NULL);
#endif
    //----------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------
    // ID读取
    id_extract(&HADM_IDs, &HADM_IDs_size, MPI_rank, MPI_size);

#ifdef TEST1
    struct timeval temp;
    gettimeofday(&temp, NULL);
    fprintf(log_file, "%d: id read time: %d ms\n", MPI_rank, time_diff_ms(begintime, temp));
    fflush(log_file);
    gettimeofday(&begintime, NULL);
#endif
    //----------------------------------------------------------------------------------------
    if (MPI_rank != 0)
    {
        // 读取表到内存中
        hash_table_len = ID_LEN;
        ADMISSIONS_table_read();
        PATIENTS_table_read();
        PRESCRIPTION_table_read();

        FEATURES_TABLE = (struct EVENTS_DATA_TABLE *)malloc(hash_table_len * sizeof(struct EVENTS_DATA_TABLE));
        memset(FEATURES_TABLE, 0, hash_table_len * sizeof(struct EVENTS_DATA_TABLE));

        LABEVENTS_table_read();
        CHARTEVENTS_table_read();
        OUTPUTEVENTS_table_read();

#ifdef MIMICIV
        ICUSTAY_table_read();
#elif defined(MIMICIII)
        TRANSFER_table_read();
        NOTE_table_read();
#endif
    }
    else
        printf("wait...\n");
    // 在此同步
    MPI_Barrier(MPI_COMM_WORLD);
    if (MPI_rank == 0)
        printf("ok\n");
#ifdef TEST1
    gettimeofday(&temp, NULL);
    fprintf(log_file, "%d: p_read time: %d ms\n", MPI_rank, time_diff_ms(begintime, temp));
    fflush(log_file);

    gettimeofday(&begintime, NULL);
    fprintf(log_file, "HID size: %d\n", HADM_IDs_size);
    fprintf(log_file, "mem: %dKB\n", get_current_mem());
    fflush(log_file);
    // 向日志文件输出内存占用
#endif
    MPI_Request *mpi_request;
    mpi_request = (MPI_Request *)malloc(sizeof(MPI_Request) * MPI_size);
    if (MPI_rank == 0)
    {
        int i;
        while (1)
        {
            printf("waiting task...\n");
            if ((connfd) = accept(listenfd, (struct sockaddr *)NULL, NULL) == -1)
            {
                printf("recv fail\n");
                continue;
            }
            int recv_msg_size = recv(connfd, recv_buffer, NET_RECV_MAX_LEN, 0);
            printf("recv: %d bytes\n", recv_msg_size);
            char finished_signal = 0x0; // 结束类型标志
            // 用于socket异常时的中止传输标志
            int stop_flag = 1;
            // 当数据包长度大于最小长度时才会处理
            if (recv_msg_size > 48)
            {
#ifdef TEST1
                // 计时
                gettimeofday(&begintime, NULL);
#endif
                struct task new_task = data_unpack(recv_buffer, recv_msg_size);
                // 发送继续信号
                for (i = 1; i < MPI_size; i++)
                    MPI_Send(&continue_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                task_send(MPI_size, new_task);
                task_free(&new_task);
                // 等待提取进程提取完成
                int extracter_signal = 0;
                for (i = 1; i < MPI_size; i++)
                    MPI_Irecv(&extracter_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &mpi_request[i]);
                // 读取数据并发送
                int *recv_flag = (int *)malloc(sizeof(int) * MPI_size);
                memset(recv_flag, 0, sizeof(int) * MPI_size);
                while (1)
                {
                    int finish_flag = recv_flag[1];
                    for (i = 1; i < MPI_size; i++)
                    {
                        int flag;
                        if (recv_flag[i] == 0 && MPI_Test(&mpi_request[i], &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS && flag == 1)
                        {
                            stop_flag = p_recv_data(i, connfd, stop_flag);
                            recv_flag[i] = 1;
                        }
                        finish_flag &= recv_flag[i];
                    }
                    if (finish_flag)
                        break;
                }
                free(recv_flag);
                int end_flag = 0;
                if (stop_flag != 2)
                    if (send(connfd, &end_flag, 4, 0) <= 0)
                        stop_flag = 2;
#ifdef TEST1
                gettimeofday(&temp, NULL);
                int time_c = time_diff_ms(begintime, temp);
                printf("\ntime: %d ms  mem: %dKB\n", time_c, get_current_mem());
                fprintf(log_file, "%d: time: %d ms  mem: %dKB\n", MPI_rank, time_c, get_current_mem());
                fflush(log_file);
#endif
                finished_signal = 0x1; // 正常结束
            }
            else if (recv_msg_size == 1 && recv_buffer[0] == 1)
            {
                // 单独收到一个1为结束信号
                break;
            }
            else
                finished_signal = 0x2; // 错误的task
            // 通过socket发送提取完成的信号
            if (finished_signal != 0x1 || stop_flag != 2)
                send(connfd, &finished_signal, 1, 0);
            close(connfd);
        }

        close(listenfd);
        // 终止
        for (i = 1; i < MPI_size; i++)
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    else
    {
        int recv_signal = -2;
        MPI_Recv(&recv_signal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        while (recv_signal == continue_signal)
        {
            struct task new_task;
            task_recv(&new_task);
#ifdef TEST1
            // 计时
            gettimeofday(&begintime, NULL);
#endif
            char **ext_results = (char **)malloc(HADM_IDs_size * sizeof(char *));
            memset(ext_results, 0, HADM_IDs_size * sizeof(char *));
            int *ext_ids = (int *)malloc(HADM_IDs_size * sizeof(int));
            memset(ext_ids, -1, HADM_IDs_size * sizeof(int));
            int result_size = 0;
            int i = 0;

            for (i = 0; i < HADM_IDs_size; i++)
            {
                if (new_task.ICD_list_size == 0 || is_ICD_in_list(HADM_IDs[i], new_task.ICD_list, new_task.ICD_list_size) != 0)
                {
                    extract(HADM_IDs[i].HADM_ID, HADM_IDs[i].SUBJECT_ID, new_task, &ext_results[result_size]);
                    ext_ids[result_size] = HADM_IDs[i].HADM_ID;
                    result_size += 1;
                }
            }
#ifdef TEST1
            gettimeofday(&temp, NULL);
            int time_c = time_diff_ms(begintime, temp);
            fprintf(log_file, "ext finish %d: time: %d ms  mem: %dKB\n", MPI_rank, time_c, get_current_mem());
#endif
            // 向主进程发送提取结果
            // 提取结束时向主进程发送消息
            int finish_signal = 1;
            MPI_Isend(&finish_signal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &mpi_request[MPI_rank]);
            p_send_data(ext_results, ext_ids, result_size);
            // 本轮提取结束, 回收资源
            task_free(&new_task);
            for (i = 0; i < result_size; i++)
                free(ext_results[i]);
            free(ext_results);
            free(ext_ids);
#ifdef TEST1
            gettimeofday(&temp, NULL);
            time_c = time_diff_ms(begintime, temp);
            fprintf(log_file, "send finish %d: time: %d ms  mem: %dKB\n", MPI_rank, time_c, get_current_mem());
            fflush(log_file);
#endif
            // 接收下一步指令(结束或继续)
            MPI_Recv(&recv_signal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
#ifdef TEST1
    fclose(log_file);
#endif
    MPI_Finalize();
    return 0;
}