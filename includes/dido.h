/*
 * @Description: 继电器和开关量 DI DO 控制和状态获取
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-03-10 11:12:03
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-03-10 17:25:36
 */

#ifndef _DIDO_H_
#define _DIDO_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gpiod.h"

int DO_Control(int status);
int DI_Status(void);

#endif