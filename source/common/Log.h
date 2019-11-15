#ifndef __LOG__
#define __LOG__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  
#include <stdint.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdarg.h>

#define LOG(fmt, ...) printf("%s "fmt, logTime(), ##__VA_ARGS__)
#define LOGERROR(fmt, ...) printf("%s "fmt" :%s\n", logTime(), ##__VA_ARGS__, strerror(errno))
static char sprint_buf[1024];
char*   logTime(void);
char *  get_cur_time();


#endif