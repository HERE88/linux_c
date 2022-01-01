#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <malloc.h>

#include <sys/stat.h>

#include <semaphore.h>
#include <termios.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>


#include <sys/socket.h>
#include <sys/un.h>

#include <dirent.h>


#include <linux/netlink.h>
#include <arpa/inet.h>
#include <ctype.h>


#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#include <math.h>

#include <stdbool.h>

#include <sys/time.h>

#include <sys/resource.h>  // setrlimit, add 20210917

#if !defined(OPT_PLATFORM_WIN32)
    typedef void                                    VOID;
    typedef char                                    CHAR;
#if !defined(OPT_SFW_EXTERNAL_TYPE_DEF)
    typedef signed char                             BOOL;
#endif
#endif
    typedef signed char                             SINT8;
    typedef signed short                         SINT16;
    typedef signed  int                         SINT32;
#ifdef WIN32
    typedef double                                  SINT64;
#else
    typedef signed long long                        SINT64;
#endif
    typedef unsigned char                           UINT8;
    typedef unsigned short                       UINT16;
#if !defined(OPT_PLATFORM_WIN32)
    typedef unsigned  int                       UINT32;
#ifdef WIN32
    typedef double                                  UINT64;
#else
    typedef unsigned long long                      UINT64;
#endif
#endif
/*********************************************INS BELOW*********************************************/

#define LOG_INFO_E(x) LOG_PRINT x
#define LOG_PRINT(...) printf(__VA_ARGS__)


void PRINT_INFO(char *format, ...)
{
    va_list ap;
    va_start(ap,format);
    vprintf(format,ap);
    va_end(ap);
}

/*********************************************INS BELOW*********************************************/


void tv_sub(struct timeval *out, struct timeval *in)
{
    if ( (out->tv_usec -= in->tv_usec) < 0) {   /* out -= in */
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}


int main(int argc, char *argv[])
{
    int ret = 0;

    struct timeval tv0,tv1; // Add 20211021.
    double clockus;

    int temp;

    int log_fd = open("wang1022.txt", O_WRONLY | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR);
    dup2(log_fd, STDOUT_FILENO);
    dup2(log_fd, STDERR_FILENO);

    printf("dup2 success!\n");

    sleep(1);

    fsync(log_fd);

    gettimeofday(&tv0, NULL);
    usleep(15000);
    gettimeofday(&tv1, NULL);
    tv_sub(&tv1, &tv0);
    clockus = tv1.tv_sec * 1000000.0 + tv1.tv_usec;
    if(clockus>10000)
    {
        printf("111clockus1=%.1fus\n", clockus);
    }

    temp  = open("aaa/a.txt", O_WRONLY | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR);
    if(temp <=0)  
    {
        perror("aaa/a.txt");
        printf("perror end!\n");
        return -1;
    }
    return 0;
}
