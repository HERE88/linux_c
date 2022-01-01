#define _GNU_SOURCE

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

#include <sys/syscall.h>   // SYS_gettid, add 20211029

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
#define LOG_PRINT(...) PRINT_INFO(__VA_ARGS__)


void PRINT_INFO(char *format, ...)
{
    char new_fm[128];
    int len;
    va_list ap;

    len=strlen(format);
    va_start(ap,format);

    strncpy(new_fm,format,sizeof(new_fm)-1);
    if(new_fm[len-1] != '\n')
    {
        new_fm[len]='\n';
        new_fm[len+1]=0;
    }

    vprintf(new_fm,ap);
    va_end(ap);
}

/*********************************************INS BELOW*********************************************/
pthread_attr_t thread_attr;
pthread_t recv_tid;

void * test_proc(void * param)
{
    usleep(1000*1000);
    
    LOG_PRINT("thd  getpid:%d SYS_getttid:%d", getpid(), syscall(SYS_gettid));
    usleep(0xffffffff);

}

int main(int argc, char *argv[])
{
    int ret_val = 0;

    ret_val = pthread_attr_init(&thread_attr);
    if (0 != ret_val)
    {
            LOG_PRINT("pthread_attr_init failed.");
            return -1;
    }

    ret_val = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (0 != ret_val)
    {
            LOG_PRINT("pthread_attr_setdetachstate failed");
            return -1;
    }

    ret_val = pthread_create(&recv_tid, &thread_attr, test_proc, NULL);
    if (0 != ret_val)
    {
            LOG_PRINT("Thread creation failed");
            return -1;
    }
    pthread_attr_destroy(&thread_attr);
    
    LOG_PRINT("main getpid:%d SYS_getttid:%d", getpid(), syscall(SYS_gettid));


    usleep(0xffffffff);
    return 0;

}
