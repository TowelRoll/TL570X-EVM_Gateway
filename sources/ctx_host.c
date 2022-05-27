#include "ctx_host.h"

//主机线程 用于host接收
static void *ctx_host_rec(void *arg);
static void *ctx_host_tcp_rec(void *arg);
static uint8_t ctx_data_deal(uint8_t *query, int header_length, modbus_mapping_t *mb_mapping);

void ctx_host_tcp_init(ModbusStru_tcp *mt)
{
    mt->ctx = modbus_new_tcp(mt->ip, mt->port);
    if (mt->ctx == NULL)
    {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return;
    }
    mt->server_socket = modbus_tcp_listen(mt->ctx, 1);

    modbus_set_debug(mt->ctx, 0);
}

void ctx_host_tcp_deinit(ModbusStru_tcp *mt)
{
    modbus_free(mt->ctx);
}

//主机开始工作，开启线程
void ctx_host_tcp_run(ModbusStru_tcp *mt)
{
    int res;
    res = pthread_create(&mt->thread, NULL, ctx_host_tcp_rec, (void *)mt);
    if (res != 0)
    {
        printf("%s create thread fail\n", NOW_TIME);
        exit(res);
    }

    mt->ctx_host_tcp_status = CTX_HOST_TCP_RUNNING;
}

//主机停止工作，关闭线程
void ctx_host_tcp_stop(ModbusStru_tcp *mt)
{
    mt->ctx_host_tcp_status = CTX_HOST_TCP_END;
}

//主机线程 用于host接收
static void *ctx_host_tcp_rec(void *arg)
{
    ModbusStru_tcp *ms_temp = (ModbusStru_tcp *)arg;

    // Host
    while (1)
    {
        modbus_tcp_accept(ms_temp->ctx, &ms_temp->server_socket);

        while (1)
        {
            uint8_t query[256];
            int header_length = modbus_get_header_length(ms_temp->ctx);
            int rc = modbus_receive(ms_temp->ctx, query);
            // printf("%d\n", modbus_get_header_length(ms_temp->ctx));
            if (rc > 0)
            {
                if(ctx_data_deal(query, header_length, ms_temp->mb_mapping))
                    modbus_reply(ms_temp->ctx, query, rc, ms_temp->mb_mapping);
            }
            else if (rc == -1 && errno == 11)
            {
                modbus_flush(ms_temp->ctx);
                fprintf(stderr, "%s ctx_tcp_host:Failed to modbus_receive:%s\n", NOW_TIME, modbus_strerror(errno));
            }
            else if (rc == -1 && errno == 104) // 104 connect error
            {
                //
                modbus_flush(ms_temp->ctx);
                fprintf(stderr, "%s ctx_tcp_host:Failed to modbus_receive:%s\n", NOW_TIME, modbus_strerror(errno));
                break;
            }
            // printf("%d %d\n", rc,errno);
            usleep(50000);
        }

        if (ms_temp->ctx_host_tcp_status == CTX_HOST_TCP_END)
            pthread_exit(NULL);
    }

    pthread_join(ms_temp->thread, NULL); //等待线程结束
}

//主机初始化，设置串口数据等
void ctx_host_init(ModbusStru_host *mh)
{
    if (mh->baud != 1200 && mh->baud != 2400 && mh->baud != 4800 && mh->baud != 9600 &&
        mh->baud != 19200 && mh->baud != 38400 && mh->baud != 57600 && mh->baud != 115200)
        mh->baud = 9600;
    if (mh->parity != 'N' && mh->parity != 'O' && mh->parity != 'E')
        mh->parity = 'N';
    if (mh->databits != 5 && mh->databits != 6 && mh->databits != 7 && mh->databits != 8)
        mh->databits = 8;
    if (mh->stopbits != 1 && mh->stopbits != 2)
        mh->stopbits = 1;

    /***************************RTU2 Host*******************************/
    mh->ctx = modbus_new_rtu(mh->port_name, mh->baud, mh->parity, mh->databits, mh->stopbits); //以串口的方式创建libmobus实例,并设置参数
    if (mh->ctx == NULL)                                                                       //
    {
        fprintf(stderr, "%s ctx_host:Unable to allocate libmodbus contex \n", NOW_TIME);
        return;
    }
    modbus_set_debug(mh->ctx, 0); //设置1可看到调试信息
    modbus_set_slave(mh->ctx, 1);

    if (modbus_connect(mh->ctx) == -1) //等待连接设备
    {
        fprintf(stderr, "%s ctx_host:Connection failed:%s\n", NOW_TIME, modbus_strerror(errno));
        return;
    }
}

//主机关闭
void ctx_host_deinit(ModbusStru_host *mh)
{
    // modbus_mapping_free(mh->mb_mapping);//释放map
    modbus_close(mh->ctx); //关闭modbus连接
    modbus_free(mh->ctx);  //释放modbus资源，使用完libmodbus需要释放掉
}

//主机寄存器地址申请
modbus_mapping_t *ctx_host_map(ModbusStru_host *mh, void *shm, uint8_t *bits, uint8_t *input_bits)
{
    /***************************************************************/
    mh->mb_mapping = modbus_mapping_new_start_address(0, 1, 0, 1, 0, 0x9999, 0, 0); // 0-9999 地址
    if (mh->mb_mapping == NULL)
    {
        fprintf(stderr, "%s Failed to allocate the mapping:%s\n", NOW_TIME, modbus_strerror(errno));
        return NULL;
    }

    mh->mb_mapping->tab_registers = (uint16_t *)shm;
    mh->mb_mapping->tab_bits = bits;
    mh->mb_mapping->tab_input_bits = input_bits;

    return mh->mb_mapping;
}

//主机开始工作，开启线程
void ctx_host_run(ModbusStru_host *mh)
{
    int res;
    res = pthread_create(&mh->thread, NULL, ctx_host_rec, (void *)mh);
    if (res != 0)
    {
        printf("%s create thread fail\n", NOW_TIME);
        exit(res);
    }

    mh->ctx_host_status = CTX_HOST_RUNNING;
}

//主机停止工作，关闭线程
void ctx_host_stop(ModbusStru_host *mh)
{
    mh->ctx_host_status = CTX_HOST_END;
}

//主机线程 用于host接收
static void *ctx_host_rec(void *arg)
{
    ModbusStru_host *mh_temp = (ModbusStru_host *)arg;

    while (1)
    {
        uint8_t query[256];

        int rc = modbus_receive(mh_temp->ctx, query);
        int header_length = modbus_get_header_length(mh_temp->ctx);
        query[0] = modbus_get_slave(mh_temp->ctx);
        if (rc > 0)
        {
            if(ctx_data_deal(query, header_length, mh_temp->mb_mapping))
                modbus_reply(mh_temp->ctx, query, rc, mh_temp->mb_mapping);
        }
        else
        {
            modbus_flush(mh_temp->ctx);
            fprintf(stderr, "%s ctx_host:Failed to modbus_receive:%s\n", NOW_TIME, modbus_strerror(errno));
        }

        usleep(50000);
    }

    pthread_join(mh_temp->thread, NULL); //等待线程结束
}

/**
 * @description: 处理modbus 主机回复
 * @param {uint8_t} *query
 * @param {int} header_length
 * @return {*}
 * @author: Towel Roll
 */
static uint8_t ctx_data_deal(uint8_t *query, int header_length, modbus_mapping_t *mb_mapping)
{
    if (query[header_length] == 0x06 || query[header_length] == 0x10)
    {
        uint16_t reg_addr = query[header_length + 1] * 0x100 + query[header_length + 2];
        char tb_name[32] = "";
        if (0x8000 <= reg_addr && reg_addr < 0x8100)
            strcpy(tb_name, "systemParams");
        else if (0x8100 <= reg_addr && reg_addr < 0x8200)
            strcpy(tb_name, "modbusParams4851");
        else if (0x8200 <= reg_addr && reg_addr < 0x8300)
            strcpy(tb_name, "modbusParams4852");
        else if (0x8300 <= reg_addr && reg_addr < 0x8400)
            strcpy(tb_name, "modbusParamsLora1");
        else
            return 0;

        DB_Update(mb_mapping->tab_registers, tb_name);
    }
    else if (query[header_length] == 0x02)
    {
        mb_mapping->tab_input_bits[0] = !DI_Status();
    }
    else if (query[header_length] == 0x05)
    {
        if (query[header_length + 1] + query[header_length + 2] == 0)
        {
            if (query[header_length + 3] == 0xFF && query[header_length + 4] == 0)
                DO_Control(1);
            else if (query[header_length + 3] == 0 && query[header_length + 4] == 0)
                DO_Control(0);
            else
                return 0;
        }
    }
    else if (query[header_length] == 0x0F)
    {
        if (query[header_length + 1] + query[header_length + 2] == 0 && query[header_length + 5] == 2)
        {
            if (query[header_length + 6] == 0xFF && query[header_length + 7] == 0)
                DO_Control(1);
            else if (query[header_length + 6] == 0 && query[header_length + 7] == 0)
                DO_Control(0);
            else
                return 0;
        }
    }

    return 1;
}