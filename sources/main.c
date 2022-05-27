/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-01-07 11:03:56
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-03-15 13:53:08
 */

/*
 *                                                     __----~~~~~~~~~~~------___
 *                                    .  .   ~~//====......          __--~ ~~
 *                    -.            \_|//     |||\\  ~~~~~~::::... /~
 *                 ___-==_       _-~o~  \/    |||  \\            _/~~-
 *         __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
 *     _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
 *   .~       .~       |   \\ -_    /  /-   /   ||      \   /
 *  /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 *  |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
 *           '         ~-|      /|    |-~\~~       __--~~
 *                       |-~~-_/ |    |   ~\_   _-~            /\
 *                            /  \     \__   \/~                \__
 *                        _--~ _/ | .-~~____--~-/                  ~~==.
 *                       ((->/~   '.|||' -_|    ~~-/ ,              . _||
 *                                  -_     ~\      ~~---l__i__i__i--~~_/
 *                                  _-~-__   ~)  \--______________--~~
 *                                //.-~~~-~_--~- |-------~~~~~~~~
 *                                       //.-~~~--\
 *                       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *                               神兽保佑            永无BUG
 */

#include "main.h"


int main(int argc, char *argv[])
{
	int s = -1;
    uint32_t sec, usec;

	//共享内存
	int shmid;
	shmid = shmget((key_t)1234, 0x9999*2, 0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "%s:shmget failed\n", NOW_TIME);
		exit(EXIT_FAILURE);
	}
	//将共享内存连接到当前进程的地址空间
	void *shm = shmat(shmid, (void*)0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "%s:shmat failed\n", NOW_TIME);
		exit(EXIT_FAILURE);
	}
	printf("%s:Memory attached at %p\n", NOW_TIME, shm);
	memset(shm, 0, 0x9999*2);

	//ctx
	ModbusStru_serial ms_rs4851 = 	{NULL, "/dev/ttymxc1", 9600, 'N', 8, 1, (ModbusStru_port *)(shm + 0x8000*2), 0,
	 CTX_SLAVE_END, (uint16_t *)shm, (ModbusStru_params *)(shm + 0x8100*2), "modbusParams4851"};
	ModbusStru_serial ms_rs4852 = 	{NULL, "/dev/ttymxc2", 9600, 'N', 8, 1, (ModbusStru_port *)(shm + 0x8003*2),0,
	 CTX_SLAVE_END, (uint16_t *)shm + 0x1000, (ModbusStru_params *)(shm + 0x8200*2), "modbusParams4852"};
	ModbusStru_serial ms_lora1 = 	{NULL, "/dev/ttymxc4", 9600, 'N', 8, 1, (ModbusStru_port *)(shm + 0x8006*2),0, 
	 CTX_SLAVE_END, (uint16_t *)shm + 0x2000, (ModbusStru_params *)(shm + 0x8300*2), "modbusParamsLora1"};

	ModbusStru_serial ms[3] = {ms_rs4851, ms_rs4852, ms_lora1};

	ModbusStru_host mh_host = {NULL, NULL, "/dev/ttymxc3", 9600, 'N', 8, 1, (uint16_t *)shm + 0x8009, 0, 0};
	uint8_t m_bits = 0, m_input_bits = 0;
	ctx_host_init(&mh_host);
	ctx_host_map(&mh_host, shm, &m_bits, &m_input_bits);
	ctx_host_run(&mh_host);

	ModbusStru_tcp mt_rj451 = { NULL, 0, mh_host.mb_mapping, "0", MODBUS_TCP_DEFAULT_PORT, 0, 0};
	ctx_host_tcp_init(&mt_rj451);
	ctx_host_tcp_run(&mt_rj451);

	/*db*/
	DB_Open();
	DB_SelectModbus(&ms_rs4851);
	DB_SelectModbus(&ms_rs4852);
	DB_SelectModbus(&ms_lora1);
	
	DB_SelectSouthPortParams(ms);
	if(DB_SelectNorthPortParams(&mh_host.baud, &mh_host.parity, &mh_host.stopbits, &mh_host.databits) == 1)
	{
		mh_host.portParamsDest[0] = mh_host.baud / 100;
		mh_host.portParamsDest[1] = mh_host.parity << 8;
		mh_host.portParamsDest[2] = (mh_host.databits << 8) + mh_host.stopbits;
		// printf("%d %d %d %d\n", mh_host.baud, mh_host.parity, mh_host.stopbits, mh_host.databits);
		ctx_host_init(&mh_host);
	}

	for(int i=0;i<3;i++)
	{
		ctx_slave_init(&ms[i]);
		ctx_slave_run(&ms[i]);
	}
	
	/*mqtt*/
	mqtt_client_t *client = NULL;
    client = mqtt_lease();
    mqtt_set_host(client, "163.179.101.20");
    mqtt_set_port(client, "4080");
    mqtt_set_clean_session(client, 1);
	mqtt_set_write_buf_size(client, 1024 * 10);
    mqtt_connect(client);
	mqtt_modbus_Init(client);

	/******************************************************************/
	while(1)
	{
    	// printf("\n----------------\n");
		DB_SelectModbus(&ms_rs4851);	//读取数据库信息，查看设置是否改变
		DB_SelectModbus(&ms_rs4852);
		DB_SelectModbus(&ms_lora1);
		DB_SelectSouthPortParams(ms);
		if(DB_SelectNorthPortParams(&mh_host.baud, &mh_host.parity, &mh_host.stopbits, &mh_host.databits) == 1)
		{
			mh_host.portParamsDest[0] = mh_host.baud / 100;
			mh_host.portParamsDest[1] = mh_host.parity << 8;
			mh_host.portParamsDest[2] = (mh_host.databits << 8) + mh_host.stopbits;
			// printf("%d %d %d \n", mh_host.portParamsDest[0], mh_host.portParamsDest[1], mh_host.portParamsDest[2]);
			ctx_host_init(&mh_host);
		}

		mqtt_publish_handle1(client, ms, sizeof(ms)/sizeof(ModbusStru_serial));

		// 
		sleep(3);
	}

	mqtt_disconnect(client);
	mqtt_release(client);

	for(int i=0;i<3;i++)
	{
		ctx_slave_stop(&ms[i]);
		ctx_slave_deinit(&ms[i]);	
	}
	ctx_host_stop(&mh_host);
	ctx_host_deinit(&mh_host);
	ctx_host_tcp_stop(&mt_rj451);
	ctx_host_tcp_deinit(&mt_rj451);
	
 	DB_Close();

	//把共享内存从当前进程中分离
	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	//删除共享内存
	if(shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}