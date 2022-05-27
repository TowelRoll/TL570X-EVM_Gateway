/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-01-07 11:03:56
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-03-11 11:15:24
 */
#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "db.h"
#include "ctx_slave.h"
#include "ctx_host.h"
#include "mqtt.h"

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned long long    uint64_t;



#endif