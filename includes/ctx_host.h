/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-01-07 11:03:56
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-03-11 10:50:49
 */
#ifndef _CTX_HOST_
#define _CTX_HOST_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "modbus.h"  //modbus动态库文件
#include "modbus-tcp.h"
#include "mydatetime.h"
#include "db.h"
#include "dido.h"

#define CTX_HOST_END 0
#define CTX_HOST_RUNNING 1

#define CTX_HOST_TCP_END 0
#define CTX_HOST_TCP_RUNNING 1

typedef struct 
{
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    //
    char port_name[20];
    uint32_t baud;
    char parity;
    uint8_t databits;
    uint8_t stopbits;
    //
    uint16_t *portParamsDest;
    //
    pthread_t thread;
    uint8_t ctx_host_status;//线程状态
}ModbusStru_host;

typedef struct 
{
    modbus_t *ctx;
    int server_socket;
    modbus_mapping_t *mb_mapping;
    //
    char ip[20];
    int port;
    //
    pthread_t thread;
    uint8_t ctx_host_tcp_status;//线程状态
}ModbusStru_tcp;

//主机初始化，设置串口数据等
void ctx_host_init(ModbusStru_host *mh);
//主机关闭
void ctx_host_deinit(ModbusStru_host *mh);
//主机寄存器地址申请
modbus_mapping_t *ctx_host_map(ModbusStru_host *mh, void *shm, uint8_t *bits, uint8_t *input_bits);
//主机开始工作
void ctx_host_run(ModbusStru_host *mh);
//主机停止工作，关闭线程
void ctx_host_stop(ModbusStru_host *mh);

void ctx_host_tcp_init(ModbusStru_tcp *mt);

void ctx_host_tcp_deinit(ModbusStru_tcp *mt);
//主机开始工作，开启线程
void ctx_host_tcp_run(ModbusStru_tcp *mt);
//主机停止工作，关闭线程
void ctx_host_tcp_stop(ModbusStru_tcp *mt);

#endif