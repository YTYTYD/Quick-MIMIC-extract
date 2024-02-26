#include "mpi.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern const int CSV_STR_MAX_LEN;

int p_recv_data(int MPI_rank, int connfd, int stop_flag);

void p_send_data(char **result, int *h_ids, int result_size);