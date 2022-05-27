/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-01-07 11:03:56
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-03-04 13:49:54
 */
#ifndef _DB_H
#define _DB_H

#include "sqlite3.h"
#include "ctx_slave.h"
#include "ctx_host.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DB_NAME "/home/gateway/db/my.db"


void DB_Open();

void DB_Close();

void DB_Update(void *shm, char *tb_name);

void DB_SelectModbus(ModbusStru_serial *ms);

void DB_SelectSouthPortParams(ModbusStru_serial ms[]);

uint8_t DB_SelectNorthPortParams(uint32_t *baud, char *parity, uint8_t *stopbits, uint8_t *databits);

#endif