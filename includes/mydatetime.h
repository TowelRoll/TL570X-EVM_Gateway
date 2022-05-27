/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-02-28 09:20:32
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-02-28 10:06:39
 */
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define NOW_TIME gettime(ntime)
static char ntime[20];

//当前机器时间
char* gettime(char *nowtime);