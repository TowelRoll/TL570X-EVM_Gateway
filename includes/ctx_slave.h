/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-01-12 11:42:05
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-03-02 08:36:50
 */
#ifndef _CTX_SLAVE_H_
#define _CTX_SLAVE_H_


#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "modbus.h"  //modbus动态库文件
#include "mydatetime.h"


#define ModbusReadArrayMax 40 // 0 ～ 64  修改的话需对应数据库

#define CTX_SLAVE_END 0
#define CTX_SLAVE_RUNNING 1

typedef struct
{
    uint16_t flag;
    uint8_t nb;
    uint8_t addr;
    uint16_t reg;
}ModbusStru_params;

typedef struct
{
    uint16_t baud;
    uint8_t sta;
    uint8_t parity;
    uint8_t stop_bit;
    uint8_t data_bit;
}ModbusStru_port;

typedef struct
{
    //Modbus设备类
    modbus_t *ctx_slave ;
    //串口设置参数
    char name[32];
    uint32_t baud;
    char parity;
    uint8_t data_bit;
    uint8_t stop_bit;
    //
    ModbusStru_port *mp; //方便modbus读取
    //线程
    pthread_t thread;
    uint8_t ctx_slave_status;//线程状态
    //数据存储位置指针
    uint16_t *dest;
    //modbus命令参数
    ModbusStru_params *modbus_set_array; //40
    //数据库表名称
    char db_name[32];
}ModbusStru_serial;



//从机初始化，设置串口数据等
void ctx_slave_init(ModbusStru_serial *ms);
//从机关闭
void ctx_slave_deinit(ModbusStru_serial *ms);
//从机开始工作
void ctx_slave_run(ModbusStru_serial *ms);
//从机停止工作，关闭线程
void ctx_slave_stop(ModbusStru_serial *ms);


#endif