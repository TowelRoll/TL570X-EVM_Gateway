/*
 * @Description: 继电器和开关量 DI DO 控制和状态获取
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-03-10 11:11:28
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-03-11 09:42:56
 */

#include "dido.h"



int DO_Control(int status)
{
    FILE *fstream = NULL;
    char cmd[16] = "gpioset 0 0=";
    cmd[12] = status + '0';

    if(NULL == (fstream = popen(cmd, "r")))
    {
        fprintf(stderr, "execute command failed: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int DI_Status(void)
{
    FILE *fstream = NULL;
    char cmd[16] = "gpioget 0 2";

    char buf[32];
    memset(buf, 0, sizeof(buf));

    if(NULL == (fstream = popen(cmd, "r")))
    {
        fprintf(stderr, "execute command failed: %s", strerror(errno));
        return -1;
    }

    while(NULL != fgets(buf, sizeof(buf), fstream));

    pclose(fstream);

    return atoi(buf);
}
