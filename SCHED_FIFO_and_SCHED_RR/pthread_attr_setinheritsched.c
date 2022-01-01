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


FILE * openLogFile2 (const char *pathname);
void setupOutput2();
void rotateLogs2();
int printBinary2(void *bufPtr, size_t bufLen);

int openLogFile (const char *pathname);
void setupOutput();
void rotateLogs();
int printBinary(void *bufPtr, size_t bufLen);

int get_sched_param_of_pthread();


/* define for Binary I/O */
#define DEFAULT_MAX_ROTATED_LOGS2 20
const char * g_outputDirName2 = "log";
const char * g_outputFileName2 = "test-pc5-tool.log";
// 0 means "no log rotation"
size_t g_logRotateSizeKBytes2 = 1024*1024;  /* Maximun bytes that saved to each log file, size:KB */
// 0 means "unbounded"
size_t g_maxRotatedLogs2 = DEFAULT_MAX_ROTATED_LOGS2;  /* Maximun number of log files. */
FILE *g_outFP = NULL;
size_t g_outByteCount2 = 0;

pthread_mutex_t w_lock = PTHREAD_MUTEX_INITIALIZER;



#define DEFAULT_MAX_ROTATED_LOGS 30
static const char * g_outputFileName = "test2-pc5-tool.log";
// 0 means "no log rotation"
static size_t g_logRotateSizeKBytes = 1024*1024;  /* Maximun bytes that saved to each log file, size:KB */
// 0 means "unbounded"
static size_t g_maxRotatedLogs = DEFAULT_MAX_ROTATED_LOGS;  /* Maximun number of log files. */
static int g_outFD = -1;
static size_t g_outByteCount = 0;
#define MAXLINE       1024



FILE * openLogFile2 (const char *pathname)
{
    return fopen(pathname, "ab");
}

void setupOutput2()
{
    if (g_outputFileName2 == NULL)
    {
        g_outFP = stdout;
    }
    else
    {
        g_outFP = openLogFile2 (g_outputFileName2);

        if (g_outFP == NULL)
        {
            LOG_INFO_E(("couldn't open output file. "));
        }

        int fileSize = 0;
        fseek(g_outFP, 0, SEEK_END);
        fileSize = ftell(g_outFP);

        if ((size_t) fileSize > SIZE_MAX || fileSize < 0)
        {
            fclose(g_outFP);
            LOG_INFO_E(("invalid output file stat. "));
        }

        g_outByteCount2 = fileSize;
    }
}

void rotateLogs2()
{
    int err;
    int i;

    // Can't rotate logs if we're not outputting to a file
    if (g_outputFileName2 == NULL)
    {
        return;
    }

    fclose(g_outFP);

    // Compute the maximum number of digits needed to count up to g_maxRotatedLogs in decimal.
    // eg: g_maxRotatedLogs == 30 -> log10(30) == 1.477 -> maxRotationCountDigits == 2
    int maxRotationCountDigits =
            (g_maxRotatedLogs2 > 0) ? (int) (floor(log10(g_maxRotatedLogs2) + 1)) : 0;

    for (i = g_maxRotatedLogs2 ; i > 0 ; i--)
    {
        char *file0, *file1;

        asprintf(&file1, "%s.%.*d", g_outputFileName2, maxRotationCountDigits, i);

        if (i - 1 == 0)
        {
            asprintf(&file0, "%s", g_outputFileName2);
        }
        else
        {
            asprintf(&file0, "%s.%.*d", g_outputFileName2, maxRotationCountDigits, i - 1);
        }

        if (!file0 || !file1)
        {
            LOG_INFO_E(("while rotating log files. "));
            break;
        }

        err = rename(file0, file1);

        if (err < 0 && errno != ENOENT)
        {
            LOG_INFO_E(("while rotating log files"));
        }

        free(file1);
        free(file0);
    }

    g_outFP = openLogFile2(g_outputFileName2);

    if (g_outFP == NULL)
    {
        LOG_INFO_E(("couldn't fopen output file. "));
    }

    g_outByteCount2 = 0;

}

int printBinary2(void *bufPtr, size_t bufLen)
{
    int bytesWritten = 0;
    
    pthread_mutex_lock(&w_lock);
    bytesWritten = fwrite(bufPtr, 1, bufLen, g_outFP);
    if( bytesWritten < 0 )
    {
        LOG_INFO_E(("fwrite to %s error. ", g_outputFileName2));
        return -1;
    }
    // g_outFP
    //fflush(g_outFP);

    g_outByteCount2 += bytesWritten;

    if (g_logRotateSizeKBytes2 > 0 && (g_outByteCount2 / 1024) >= g_logRotateSizeKBytes2)
    {
        fflush(g_outFP);  //flush!
        rotateLogs2();
    }
    pthread_mutex_unlock(&w_lock);
    return 0;
}

int openLogFile (const char *pathname)
{
    return open(pathname, O_WRONLY | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR);
}

void setupOutput()
{
    if (g_outputFileName == NULL)
    {
        g_outFD = STDOUT_FILENO;
    }
    else
    {
        g_outFD = openLogFile (g_outputFileName);

        if (g_outFD < 0)
        {
            LOG_INFO_E(("couldn't open output file. "));
        }

        struct stat statbuf;
        if (fstat(g_outFD, &statbuf) == -1)
        {
            close(g_outFD);
            LOG_INFO_E(("couldn't get output file stat. "));
        }

        if ((size_t) statbuf.st_size > SIZE_MAX || statbuf.st_size < 0)
        {
            close(g_outFD);
            LOG_INFO_E(("invalid output file stat. "));
        }

        g_outByteCount = statbuf.st_size;
    }
}


void rotateLogs()
{
    int err;
    int i;

    // Can't rotate logs if we're not outputting to a file
    if (g_outputFileName == NULL)
    {
        return;
    }

    close(g_outFD);

    // Compute the maximum number of digits needed to count up to g_maxRotatedLogs in decimal.
    // eg: g_maxRotatedLogs == 30 -> log10(30) == 1.477 -> maxRotationCountDigits == 2
    int maxRotationCountDigits =
            (g_maxRotatedLogs > 0) ? (int) (floor(log10(g_maxRotatedLogs) + 1)) : 0;

    for (i = g_maxRotatedLogs ; i > 0 ; i--)
    {
        char *file0, *file1;

        asprintf(&file1, "%s.%.*d", g_outputFileName, maxRotationCountDigits, i);

        if (i - 1 == 0)
        {
            asprintf(&file0, "%s", g_outputFileName);
        }
        else
        {
            asprintf(&file0, "%s.%.*d", g_outputFileName, maxRotationCountDigits, i - 1);
        }

        if (!file0 || !file1)
        {
            LOG_INFO_E(("while rotating log files. "));
            break;
        }

        err = rename(file0, file1);

        if (err < 0 && errno != ENOENT)
        {
            LOG_INFO_E(("while rotating log files"));
        }

        free(file1);
        free(file0);
    }

    g_outFD = openLogFile(g_outputFileName);

    if (g_outFD < 0)
    {
        LOG_INFO_E(("couldn't open output file. "));
    }

    g_outByteCount = 0;

}

int printBinary(void *bufPtr, size_t bufLen)
{
    int bytesWritten = 0;
    pthread_mutex_lock(&w_lock);
    bytesWritten = write(g_outFD, bufPtr, bufLen);
    if( bytesWritten < 0 )
    {
        LOG_INFO_E(("write to %s error:%d ", g_outputFileName, bytesWritten));
        return -1;
    }

    g_outByteCount += bytesWritten;

    if (g_logRotateSizeKBytes > 0 && (g_outByteCount / 1024) >= g_logRotateSizeKBytes)
    {
        rotateLogs();
    }
    pthread_mutex_unlock(&w_lock);
    return 0;
}


void tv_sub(struct timeval *out, struct timeval *in)
{
    if ( (out->tv_usec -= in->tv_usec) < 0) {   /* out -= in */
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

int get_sched_param_of_pthread()
{
    //printf sched_param of the thread
    int ret = -1;
    int policy;
    struct sched_param param;

    ret = pthread_getschedparam(pthread_self(), &policy, &param);
    if(ret != 0)
    {
        LOG_PRINT("pthread_getschedparam error");
        return ret;
    }

    switch(policy)
    {
        case SCHED_FIFO:
            LOG_PRINT("SCHED_FIFO tp");
            break;
        case SCHED_RR:
            LOG_PRINT("SCHED_RR tp");
            break;
        case SCHED_OTHER:
            LOG_PRINT("SCHED_OTHER tp");
            break;
    }
    LOG_PRINT("param.sched_priority = %d", param.sched_priority);
    return 0;
}

void pub_util_raise_thread_priority( void )
{
    struct sched_param  param;
    int    ret_val = 0;

    LOG_PRINT("pub_util_raise_thread_priority entering\n" );

    ret_val = sched_get_priority_max( SCHED_FIFO );
    if( ret_val != -1 )
    {
        param.sched_priority = ret_val - 0;
        ret_val = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        if( -1 == ret_val )
        {
            LOG_PRINT("pub_util_raise_thread_priority error ret_val = %d\n", ret_val);
        }
        else
        {
            LOG_PRINT("pub_util_raise_thread_priority sched_priority = %d\n", param.sched_priority);
        }
    }
    else
    {
        LOG_PRINT("sched_get_priority_max error");
    }
}

void * test_proc(void * param)
{
    char temp_buf[1024];
    //pub_util_raise_thread_priority();   //在线程内部提升优先级！！！--20211028.

    get_sched_param_of_pthread();

    usleep(1000*1000);
    while(1)
    {
        //printBinary2(temp_buf,1024);
        //usleep(1000*1000);
    }

}

int main(int argc, char *argv[])
{
    int ret_val = 0;

    struct sched_param param;
    int policy;

#if 1
    int ret;

    struct sched_param sd;
    cpu_set_t set;

    sd.sched_priority=40;   //设置实时优先级40
    ret = sched_setscheduler(0,SCHED_RR,&sd);  // If pid equals zero, the scheduling policy and parameters of the calling thread will be set.
    if(ret != 0)
    {
        perror("sched_setscheduler ret error"); //确保有root权限！！！--20211027
        return -1;
    }
    CPU_ZERO(&set);
    CPU_SET(0,&set);
    ret = sched_setaffinity(0,sizeof(cpu_set_t),&set);   //设置（进程的）CPU亲和力
    if(ret != 0)
    {
        perror("sched_setaffinity ret error"); //确保有root权限！！！
        return -1;
    }
#endif

#if 1
    ret_val = pthread_attr_init(&thread_attr);
    if (0 != ret_val)
    {
            LOG_PRINT("pthread_attr_init failed.");
            return -1;
    }
    
    /////////////////////////////////////////////////////////////////////////////
    ret_val = pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);   // Must be PTHREAD_EXPLICIT_SCHED!!!--20211028.
    printf("pthread_attr_setinheritsched ret: %d\n", ret_val);

    ret_val = pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
    printf("pthread_attr_setschedpolicy ret: %d\n", ret_val);

    param.sched_priority=49;
    ret_val = pthread_attr_setschedparam(&thread_attr, &param);
    printf("pthread_attr_setschedparam ret: %d\n", ret_val);

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
#endif

    usleep(1000*1000);
    while(1);

    usleep(0xfffffff);
    return 0;

}
