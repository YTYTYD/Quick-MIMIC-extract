#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "taskInit.h"
#include "utils.h"

extern int listenfd, connfd;
extern struct sockaddr_in servaddr;
extern unsigned char *recv_buffer;
extern const int NET_RECV_MAX_LEN;

int network_init();
struct task data_unpack(unsigned char *buffer, int buffer_size);