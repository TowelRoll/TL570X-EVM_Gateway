#include "db.h"

static sqlite3 *db;

static int callback(void *dat, int argc, char **argv, char **azColName)
{
	// printf("%d\n", argc);
	ModbusStru_serial *ms_temp = (ModbusStru_serial *)dat;

	int id = atoi(argv[0]) - 1;
	
	if(id > ModbusReadArrayMax)
		return 0;

	ms_temp->modbus_set_array[id].flag = atoi(argv[1]);
	ms_temp->modbus_set_array[id].addr = atoi(argv[2]);
	ms_temp->modbus_set_array[id].reg = atoi(argv[3]);
	ms_temp->modbus_set_array[id].nb = atoi(argv[4]);

	// printf("%s: %d-%d\n", NOW_TIME, ms_temp->modbus_set_array[id].flag, atoi(argv[1]));

	return 0;
}

static int cb_systemparams(void *dat, int argc, char **argv, char **azColName)
{
		// printf("%d\n", argc);
	ModbusStru_serial *ms_temp = (ModbusStru_serial *)dat;

	for(int i=0;i<3;i++)
	{
		uint8_t flag = 0;

		ms_temp[i].mp->sta = atoi(argv[0+5*i]);
		ms_temp[i].mp->baud = atoi(argv[1+5*i]) / 100;
		ms_temp[i].mp->parity = *argv[2+5*i];
		ms_temp[i].mp->stop_bit = atoi(argv[3+5*i]);
		ms_temp[i].mp->data_bit = atoi(argv[4+5*i]);

		if(ms_temp[i].baud == atoi(argv[1+5*i]));
		else{
			ms_temp[i].baud = atoi(argv[1+5*i]);
			flag = 1;
		}

		if(ms_temp[i].parity == *argv[2+5*i]);
		else{
			ms_temp[i].parity = *argv[2+5*i];
			flag = 1;
		}

		if(ms_temp[i].stop_bit == atoi(argv[3+5*i]));
		else{
			ms_temp[i].stop_bit = atoi(argv[3+5*i]);
			flag = 1;
		}

		if(ms_temp[i].data_bit == atoi(argv[4+5*i]));
		else{
			ms_temp[i].data_bit = atoi(argv[4+5*i]);
			flag = 1;
		}

		if(ms_temp[i].ctx_slave_status == atoi(argv[0+5*i]));
		else{
			ms_temp[i].ctx_slave_status = atoi(argv[0+5*i]);
		}

		if(flag == 1)
		{
			// ctx_slave_deinit(&ms_temp[i]);
			ctx_slave_init(&ms_temp[i]);
		}
			
	}

	return 0;
}

void DB_Open()
{   
	int rc;
	
	rc = sqlite3_open(DB_NAME, &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}
}

void DB_Close()
{
    sqlite3_close(db);
}

void DB_SelectModbus(ModbusStru_serial *ms)
{
    int rc;
    char sql[64] = "SELECT * from ";
	char *zErrMsg = 0;

    strcat(sql, ms->db_name);
	rc = sqlite3_exec(db, sql, callback, (void *)ms, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "DB %s successfully\n", ms->db_name);
	}
}

void DB_SelectSouthPortParams(ModbusStru_serial ms[])
{
    int rc;
    char sql[64] = "SELECT * from systemParams";
	char *zErrMsg = 0;

	rc = sqlite3_exec(db, sql, cb_systemparams, (void *)ms, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "DB SELECT successfully\n");
	}
}

uint8_t DB_SelectNorthPortParams(uint32_t *baud, char *parity, uint8_t *stopbits, uint8_t *databits)
{
	int rc = 0;
    char sql[128] = "SELECT BaudGateway,ParityGateway,StopBitsGateway,DataBitsGateway from systemParams";
	char *zErrMsg = 0;
	char **azResult;
	int nrow = 0;
	int ncolumn = 0;

	if(sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg) != SQLITE_OK)
	{
		fprintf(stderr, "SQL error:%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		// printf("%d %c %d %d \n", atoi(azResult[4]), *azResult[5], atoi(azResult[6]), atoi(azResult[7]));
		// printf("%d %c %d %d \n", *baud, *parity, *stopbits, *databits);

		if(*baud == atoi(azResult[4]));
		else{
			*baud = atoi(azResult[4]);
			rc = 1;
		}

		if(*parity == *azResult[5]);
		else{
			*parity = *azResult[5];
			rc = 1;
		}

		if(*stopbits == atoi(azResult[6]));
		else{
			*stopbits = atoi(azResult[6]);
			rc = 1;
		}

		if(*databits == atoi(azResult[7]));
		else{
			*databits = atoi(azResult[7]);
			rc = 1;
		}

		sqlite3_free_table(azResult);
	}

	return rc;
}

void DB_Update(void *shm, char *tb_name)
{
	char *zErrMsg = 0;
    char cmd[4096*3] = {0};

	if(strcmp(tb_name, "systemParams") == 0)
	{
		uint16_t *temp = (uint16_t *)shm + 0x8000;
		sprintf(cmd, "update %s set Sta4851=%d,Baud4851=%d,Parity4851='%c',StopBits4851=%d,DataBits4851=%d,"
		"Sta4852=%d,Baud4852=%d,Parity4852='%c',StopBits4852=%d,DataBits4852=%d,"
		"StaLora1=%d,BaudLora1=%d,ParityLora1='%c',StopBitsLora1=%d,DataBitsLora1=%d,"
		"BaudGateway=%d,ParityGateway='%c',StopBitsGateway=%d,DataBitsGateway=%d;", tb_name,
		 temp[1]%0x100, temp[0]*100, temp[1]/0x100, temp[2]%0x100, temp[2]/0x100,
		 temp[4]%0x100, temp[3]*100, temp[4]/0x100, temp[5]%0x100, temp[5]/0x100,
		 temp[7]%0x100, temp[6]*100, temp[7]/0x100, temp[8]%0x100, temp[8]/0x100,
		 temp[9]*100, temp[10]/0x100, temp[11]%0x100, temp[11]/0x100
		 );
		//  printf("%d\n", temp[0]*100);
		//  printf("%s\n", cmd);
	}
	else
	{
		for(int i=0;i<ModbusReadArrayMax;i++)
		{
			char s[100];
			sprintf(s, "update %s set flag=%d,addr=%d,nums=%d,register=%d where ID=%d;", tb_name,
				*((uint16_t *)shm + 0x8100+3*i), *((uint16_t *)shm + 0x8101+3*i)/0x100, 
				*((uint16_t *)shm + 0x8101+3*i)%0x100, *((uint16_t *)shm + 0x8102+3*i), i+1);
			strcat(cmd, s);	
		}
	}

    // printf("%s\n", cmd);
    if(sqlite3_exec(db, cmd, NULL, NULL, &zErrMsg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error:%s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "DB UPDATE successfully\n");
    }
}