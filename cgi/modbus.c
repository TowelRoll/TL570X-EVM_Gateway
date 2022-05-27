/*
 *    
 *   2021.9.15 消息队列发送
 * 
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>


#include "sqlite3.h"

typedef unsigned short uint16_t;

#define DB_NAME "/home/gateway/db/my.db"


char* itoa(int value, char *str)
{
    memset(str, 0, strlen(str));

    int number = 0;
    char stemp[8];

    if(value == 0)
        return "0";
    
    for(int i=0;value>0;i++)
    {
        stemp[i] = value%10;
        number++;
        value /= 10;
    }

    for(int i=0;i<number;i++)
        str[i] = stemp[number-1-i] + '0';

    str[number] = '\0';    
    
    return str;
}

static int callback(void *dat, int argc, char **argv, char **azColName)
{
    printf("%s", *argv[0]);
	return 0;
}

int main(int argc, char *argv[])
{
    int id, flag, addr, reg, nums = 0;
    char table[32];
    char header[32];
    char value[16];
    char data_post[1024];
    char *data;
    char *method;

    /*************************************/
    data = getenv("QUERY_STRING");   //getenv()读取环境变量的当前值的函数 
    method = getenv("REQUEST_METHOD");

    printf("Content-type: text/html\n\n");
    //
    if(strcmp(method, "POST") == 0)
    {
        fread(data_post, 1, 1024, stdin);

        if(sscanf(data_post, "id=%d,flag=%d,addr=%d,register=%d,nums=%d,table=%s", &id, &flag, &addr, &reg, &nums, table) == 6)
        {   //利用sscnaf（）函数的特点将环境变量分别提取出led_num和led_state这两个值
            // printf("REC: %d, %d, %d, %d, %d", id, flag, addr, reg, nums);
                /******************************************/
            sqlite3 *db;
            int rc;
            char *zErrMsg;
            
            rc = sqlite3_open(DB_NAME, &db);
            if( rc )
            {
                printf("Can't open database: %s\n\n", sqlite3_errmsg(db));
                return 0;
            }
            else
            {
                printf("Opened database successfully\n\n");
            }

            char sql[100] = "update ";
            strcat(sql, table);
            strcat(sql, " set flag=");
            char str[8];
            strcat(sql, itoa(flag, str));
            strcat(sql, ",addr=");
            strcat(sql, itoa(addr, str));
            strcat(sql, ",register=");
            strcat(sql, itoa(reg, str));
            strcat(sql, ",nums=");
            strcat(sql, itoa(nums, str));
            strcat(sql, " where ID=");
            strcat(sql, itoa(id, str));
            strcat(sql, ";");

            printf("%s\n\n", sql);

            if(sqlite3_exec(db, sql, NULL, NULL, &zErrMsg) != SQLITE_OK)
            {
                printf("SQL error:%s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
            else
            {
                printf("Update successfully\n\n");
            }

            sqlite3_close(db);
        }
        else if(sscanf(data_post, "sysparam=%[^,],sysparamvalue=%s", header, value) == 2)
        {
            sqlite3 *db;
            int rc;
            char *zErrMsg;
            
            rc = sqlite3_open(DB_NAME, &db);
            if( rc )
            {
                printf("Can't open database: %s\n\n", sqlite3_errmsg(db));
                return 0;
            }
            else
            {
                // printf("Opened database successfully\n\n");
            }

            char sql[100] = "update systemParams ";
            strcat(sql, "set ");
            strcat(sql, header);
            strcat(sql, "=");
            strcat(sql, value);
            strcat(sql, ";");

            // printf("%s\n\n", sql);

            if(sqlite3_exec(db, sql, callback, NULL, &zErrMsg) != SQLITE_OK)
            {
                printf("SQL error:%s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
            else
            {
                printf("select successfully\n\n");
            }

            sqlite3_close(db);
        }
        else
            printf("Data:%s\n\n", data);
    }
    else if(strcmp(method, "GET") == 0)
    {
        if(sscanf(data, "id=%d,table=%s", &id, table) == 2)
        {
            sqlite3 *db;
            int rc;
            char *zErrMsg;
            char **azResult;
            int nrow = 0;
            int ncolumn = 0;
            
            rc = sqlite3_open(DB_NAME, &db);
            if( rc )
            {
                printf("Can't open database: %s\n\n", sqlite3_errmsg(db));
                return 0;
            }
            else
            {
                // printf("Opened database successfully\n\n");
            }

            char sql[100] = "select flag,addr,register,nums from ";
            strcat(sql, table);
            strcat(sql, " where id=");
            char str[8];
            strcat(sql, itoa(id, str));
            strcat(sql, ";");

            // printf("%s\n\n", sql);

            if(sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg) != SQLITE_OK)
            {
                printf("SQL error:%s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
            else
            {
                char json[64] = "{\"";
                for(int i=0;i<ncolumn;i++)
                {
                    strcat(json, azResult[i]);
                    strcat(json, "\":");
                    strcat(json, azResult[i+ncolumn]);
                    
                    if(i != ncolumn - 1)
                        strcat(json, ",\"");
                }
                strcat(json, "}");
                printf("%s\n\n", json);
                sqlite3_free_table(azResult);
            }

            sqlite3_close(db);
        }
        else if(sscanf(data, "readReg=%d,readLen=%d", &reg, &nums) == 2)
        {
            int shmid;
            void *shm;

            shmid = shmget((key_t)1234, 0x9999*2, 0666 | IPC_CREAT);
            if(shmid == -1)
            {
                fprintf(stderr, "shmget failed\n");
                exit(EXIT_FAILURE);
            }

            //将共享内存连接到当前进程的地址空间
            shm = shmat(shmid, 0, 0);
            if(shm == (void*)-1)
            {
                fprintf(stderr, "shmat failed\n");
                exit(EXIT_FAILURE);
            }

            uint16_t *temp_ = (uint16_t *)shm + reg;
            char json[65535] = "{\"readReg\":";
            char temp[8];
            strcat(json, itoa(reg, temp));
            strcat(json, ",\"readLen\":");
            strcat(json, itoa(nums, temp));
            strcat(json, ",\"value\":[");
            for(int i=0;i<nums;i++)
            {
                strcat(json, itoa(temp_[i], temp));
                if(i != nums-1)
                    strcat(json, ",");
            }
            strcat(json, "]}");
            printf("%s ", json);
        }
        else if(sscanf(data, "sysparam=%s", header) == 1)
        {
            sqlite3 *db;
            int rc;
            char *zErrMsg;
            char **azResult;
            int nrow = 0;
            int ncolumn = 0;
            
            rc = sqlite3_open(DB_NAME, &db);
            if( rc )
            {
                printf("Can't open database: %s\n\n", sqlite3_errmsg(db));
                return 0;
            }
            else
            {
                // printf("Opened database successfully\n\n");
            }

            char sql[100] = "select ";
            strcat(sql, header);
            strcat(sql, " from systemParams");
            strcat(sql, ";");

            // printf("%s\n\n", sql);

            if(sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg) != SQLITE_OK)
            {
                printf("SQL error:%s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
            else
            {   
                printf("%s\n\n", azResult[1]);
                sqlite3_free_table(azResult);
            }

            sqlite3_close(db);
        }
        else
            printf("%s\n\n", data);
    }
    else
        return 0;
    // 

    // printf("Ok!\n\n");

	return 0;
}
