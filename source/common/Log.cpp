#include "Log.h"
char*   logTime(void)
{
	struct  tm      *ptm;
	struct  timeb   stTimeb;
	static  char    szTime[19];

	ftime(&stTimeb);
	ptm = localtime(&stTimeb.time);
	sprintf(szTime, "%02d-%02d %02d:%02d:%02d.%03d",
		ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, stTimeb.millitm);
	szTime[18] = 0;
	return szTime;
}
char* get_cur_time()
{
	static char s[20];
	time_t t;
	struct tm* ltime;
	time(&t);
	ltime = localtime(&t);
	strftime(s, 20, "%Y-%m-%d %H:%M:%S", ltime);
	return s;
}


int log(const char *fmt ...)
{
	va_list args;
	int n;
	sprintf(sprint_buf, "time:%s ", logTime());
	//Êä³öÄÚÈİ
	va_start(args, fmt);
	n = vsprintf(sprint_buf, fmt, args);
	va_end(args);
	return n;
}

