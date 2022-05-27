/*
 * @Description: mqtt 功能实现类
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-02-23 16:53:32
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-02-24 16:58:39
 */


#include "mqtt.h"


/**
 * @description: modbus_Rec 主题订阅处理
 * @param {void*} client
 * @param {message_data_t*} msg
 * @return {*}
 * @author: Towel Roll
 */
static void mqtt_modbusRec(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}


/**
 * @description: modbus_Send 主题发布处理
 * @param {mqtt_client_t} *client
 * @return {*}
 * @author: Towel Roll
 */
int mqtt_publish_handle1(mqtt_client_t *client, ModbusStru_serial *ms, unsigned char nums)
{ 
    for(int i=0;i<nums;i++)
    {
        char topicname[32] = "modbus_Send/";    //topic
        strcat(topicname, ms[i].db_name + 12);

        cJSON *root, *body;

        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "portname", ms[i].db_name + 12); 
        cJSON_AddItemToObject(root, "data", body = cJSON_CreateArray());
        
        for(int j=0;j<ModbusReadArrayMax;j++)
        {
            if(ms[i].modbus_set_array[j].flag == 0)
                continue;

            cJSON *list, *list_value;

            cJSON_AddItemToArray(body, list = cJSON_CreateObject());
            cJSON_AddNumberToObject(list, "reg", ms[i].modbus_set_array[j].reg);
            cJSON_AddNumberToObject(list, "nb", ms[i].modbus_set_array[j].nb);
            cJSON_AddItemToObject(list, "value", list_value = cJSON_CreateArray());
            int nums = 0;
            for(int k = 0; k < j; k++)
            {
                nums += ms[i].modbus_set_array[k].nb;
            }
            for(int k = 0; k < ms[i].modbus_set_array[j].nb; k++)
            {
                cJSON_AddItemToArray(list_value, cJSON_CreateNumber(*(ms[i].dest + nums + k)));
            }      
        }
        cJSON_AddStringToObject(root, "time", NOW_TIME);
        // printf("%s\n", cJSON_PrintUnformatted(root));
        mqtt_message_t msg;
        memset(&msg, 0, sizeof(msg));

        msg.qos = 0;
        msg.payload = (void *)cJSON_Print(root);

        mqtt_publish(client, topicname, &msg);

        free(msg.payload);
        cJSON_Delete(root);     
    }

    return 0;
}


/**
 * @description: 
 * @param {mqtt_client_t} *client
 * @return {*}
 * @author: Towel Roll
 */
void mqtt_modbus_Init(mqtt_client_t *client)
{
    mqtt_log_init();

    mqtt_subscribe(client, "modbus_Rec", QOS0, mqtt_modbusRec);
}
