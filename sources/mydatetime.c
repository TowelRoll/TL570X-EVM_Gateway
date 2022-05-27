/*
 * @Description: 
 * @Version: 1.0
 * @Autor: Towel Roll
 * @Date: 2022-02-28 09:20:20
 * @LastEditors: Towel Roll
 * @LastEditTime: 2022-02-28 09:21:39
 */
#include "mydatetime.h"

char* gettime(char *nowtime)
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	unsigned long long sec = tv.tv_sec;

	struct tm cur_tm;
	localtime_r((time_t *)&sec, &cur_tm);
	snprintf(nowtime, 20, "%d-%02d-%02d %02d:%02d:%02d",cur_tm.tm_year+1900,cur_tm.tm_mon+1,cur_tm.tm_mday,cur_tm.tm_hour,cur_tm.tm_min,cur_tm.tm_sec);

	return nowtime;
}