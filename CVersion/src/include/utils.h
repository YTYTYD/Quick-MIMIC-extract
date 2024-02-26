#pragma once
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "defines.h"

int my_atoi(char *str);

double my_atof(char *str);

void my_itoa(int n, char *result);

long long str_2_time_stamp(char *str_time);

void buff_index_move(char *buff, unsigned int *index, int n);

void str_cpy(char *buff, char *dst, unsigned int *start, char separator);

void str_cpy_2(char *buff, char *dst, unsigned int *start, char separator, char no_copy_char);

/**
 * 判断一个数n是否在数组arr中
 * 其中arr无数据的部分要填充-1
 */
int is_in_array(int n, int *arr);

int time_diff_ms(struct timeval start, struct timeval end);

/**
 * 测试用函数, 返回当前程序占用的内存, 单位KB
 */
int get_current_mem();