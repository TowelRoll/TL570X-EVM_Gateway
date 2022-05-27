/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-01-12 11:41:47
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-03-07 13:55:40
 */
#include "ctx_slave.h"


//
static void *ctx_slave_send(void *arg);

//从机初始化，设置串口数据等
void ctx_slave_init(ModbusStru_serial *ms)
{
    /* 防呆 */
    if(ms->baud != 1200 && ms->baud != 2400 && ms->baud != 4800 && ms->baud != 9600 &&
        ms->baud != 19200 &&ms->baud != 38400 && ms->baud != 57600 && ms->baud != 115200)
        ms->baud = 9600;
    if(ms->parity != 'N' && ms->parity != 'O' && ms->parity != 'E')
        ms->parity = 'N';
    if(ms->data_bit != 5 && ms->data_bit != 6 && ms->data_bit != 7 && ms->data_bit != 8)
        ms->data_bit = 8;
    if(ms->stop_bit != 1 && ms->stop_bit != 2)
        ms->stop_bit = 1;

    // printf("%d\n",ms->baud);
	/************************ RTU1 Slave*****************************/	        
	ms->ctx_slave = modbus_new_rtu(ms->name, ms->baud, ms->parity, ms->data_bit, ms->stop_bit);		//以串口的方式创建libmobus实例,并设置参数			
	if (ms->ctx_slave == NULL)                //
	{
    	fprintf(stderr, "%s ctx_slave：%s Unable to allocate libmodbus contex\n", NOW_TIME, ms->name);
    	return;
	}
	modbus_set_debug(ms->ctx_slave, 0);      //设置1可看到调试信息
    modbus_set_response_timeout(ms->ctx_slave, 0, 1000000);

	if (modbus_connect(ms->ctx_slave) == -1) //等待连接设备
	{
    	fprintf(stderr, "%s ctx_slave：%s Connection failed:%s\n", NOW_TIME, ms->name, modbus_strerror(errno));
    	return;
	}
}

//从机关闭
void ctx_slave_deinit(ModbusStru_serial *ms)
{
    modbus_close(ms->ctx_slave);  //关闭modbus连接
	modbus_free(ms->ctx_slave);   //释放modbus资源，使用完libmodbus需要释放掉
}

//从机开始工作
void ctx_slave_run(ModbusStru_serial *ms)
{
    int res;
	res = pthread_create(&ms->thread, NULL, ctx_slave_send, (void *)ms);
	if(res != 0)
    {
        fprintf(stderr, "%s: %s create thread fail\n", NOW_TIME, ms->name);
        exit(res);
    }

    ms->ctx_slave_status = CTX_SLAVE_RUNNING;
}

//从机停止工作
void ctx_slave_stop(ModbusStru_serial *ms)
{
    ms->ctx_slave_status = CTX_SLAVE_END;
}

static void *ctx_slave_send(void *arg)
{
    ModbusStru_serial *ms_temp = (ModbusStru_serial *)arg;

    while(1)
    {   
        // memset(dest, 0, 0x9999*2);//清理上一次读取的值
        for(int i=0;i<ModbusReadArrayMax;i++)
        {
            if(ms_temp->ctx_slave == NULL || ms_temp->ctx_slave_status == CTX_SLAVE_END)
                break;
            if(ms_temp->modbus_set_array[i].flag == 0)
                continue;

            int sum = 0;//偏移量
            for(int j=0;j<i;j++)
            {
                if(ms_temp->modbus_set_array[j].flag == 0)
                    continue;
                sum += ms_temp->modbus_set_array[j].nb;
            }   
            modbus_set_slave(ms_temp->ctx_slave, ms_temp->modbus_set_array[i].addr);      //设置slave ID
            // printf("%d %d %d %d \n",  ms_temp->modbus_set_array[i].flag, ms_temp->modbus_set_array[i].addr, ms_temp->modbus_set_array[i].reg, 
            // 	ms_temp->modbus_set_array[i].nb);

            int rc = modbus_read_registers(ms_temp->ctx_slave, ms_temp->modbus_set_array[i].reg, 
            	ms_temp->modbus_set_array[i].nb, ms_temp->dest);//从0地址存放
            if (rc == -1)                   //读取保持寄存器的值，可读取多个连续输入保持寄存器
            {
                fprintf(stderr, "%s ctx_slave： %s %s\n", NOW_TIME, ms_temp->name, modbus_strerror(errno)); 
            }
            else
            {
                modbus_flush(ms_temp->ctx_slave);
                // fprintf(stderr, "%s: slave %s, %d read_reegisters ok!\n", NOW_TIME, ms_temp->name, i);
            }
            
            usleep(300000);
        }
        // printf("%d %d\n", ms_temp->ctx_slave, ms_temp->ctx_slave_status);
        sleep(1);
    }
}
