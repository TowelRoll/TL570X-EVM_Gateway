/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-02-23 16:53:55
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-02-28 09:22:39
 */
#ifndef _MQTT_H_
#define _MQTT_H_

#include <stdio.h>
#include <stdint.h>

#include "mqttclient.h"
#include "cJSON.h"
#include "ctx_slave.h"
#include "mydatetime.h"

/**
 * @description: modbus_Send 主题发布处理
 * @param {mqtt_client_t} *client
 * @return {*}
 * @author: Towel Roll
 */
int mqtt_publish_handle1(mqtt_client_t *client, ModbusStru_serial ms[], unsigned char nums);

/**
 * @description: 
 * @param {mqtt_client_t} *client
 * @return {*}
 * @author: Towel Roll
 */
void mqtt_modbus_Init(mqtt_client_t *client);

#endif
