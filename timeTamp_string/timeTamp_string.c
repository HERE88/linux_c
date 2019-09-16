#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024

static char *getTimeStr(char ** bufPtr)
{
    char msgBuf[64] = {0};
    struct timeval tv;
    struct tm * tm_ptr;

    gettimeofday(&tv, NULL);
    tm_ptr = localtime(&(tv.tv_sec));

    snprintf(msgBuf, sizeof(msgBuf),"[%02d-%02d %02d:%02d:%02d.%06d]", tm_ptr->tm_mon, tm_ptr->tm_mday,
        tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, tv.tv_usec);

    //printf("msgBuf: %s\n", msgBuf);
    strncpy(*bufPtr, msgBuf, sizeof(msgBuf));

    return (*bufPtr);
}

int main(int argc, char * argv[])
{
    char tempBuf[MAXLINE] = {0};
    char *ptr = tempBuf; //传递二级指针！
    //getTimeStr(&ptr); //不能直接getTimeStr(&tempBuf)!!

    //printf("tempBuf: %s\n", tempBuf);
    while(1)
    {
        printf("return : %s\n", getTimeStr(&ptr));
        usleep(1000*1000);
    }
    return 0;
}