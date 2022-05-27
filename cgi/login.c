
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

int main(int argc, char *argv[])
{
    int id, flag, addr, reg, nums = 0;
    char name[128];
    char password[128];
    char data[1024];
    char *method;

    printf("Content-type: text/html\n\n");

    // printf("test!\n\n");

    method = getenv("REQUEST_METHOD");
    if(strcmp(method, "POST") == 0)
    {
        // printf("ok\n\n");
        
        int ret = fread(data, 1, 1024, stdin);
        if(ret <= 0)
        {
            printf("fread error: %s\n\n", ret);
            return 0;
        }
        // printf("%s\n\n", data);
        if(sscanf(data, "name=%[^,],password=%s", &name, &password) == 2)
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

            char sql[100] = "select * from login;";
            // printf("%s\n\n", sql);

            if(sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg) != SQLITE_OK)
            {
                printf("SQL error:%s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
            else
            {
                if(strcmp(name, azResult[2]) == 0 && strcmp(password, azResult[3]) == 0)
                    printf("1");
                else
                    printf("0");
                // printf("%s:%s \n %s:%s\n\n", name,azResult[2], password,azResult[3]);
                sqlite3_free_table(azResult);
            }
            sqlite3_close(db);
        } 
        else
            printf("Error\n\n");
    }
    else
        printf("Error\n\n");
    return 0;
}
