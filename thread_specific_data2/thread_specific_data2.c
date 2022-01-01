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

/*********************************************INS BELOW*********************************************/


static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;


void destructor_func(void * val_ptr)
{
    printf("thread %lx: freeing buf at %p\n", pthread_self(), val_ptr);
    
    free(val_ptr);
}

void thread_init(void)
{
    //pthread_key_create(&key, free);
    pthread_key_create(&key, destructor_func);
    
    printf("thread %lx: allocated key %d\n", pthread_self(), key);
}

char * str_accumulate(const char * s)
{
    char * acc;

    pthread_once(&init_done, thread_init);  // thread_init only called once!
    acc = (char *)pthread_getspecific(key);
    if(acc == NULL)
    {
        acc = (char *)malloc(1024);
		pthread_setspecific(key, acc);
        printf("thread %lx: allocating buf at %p\n", pthread_self(), acc);
    }

    strcat(acc, s);
    return acc;
}

void *thread_func(void * arg)
{
    char * res;
    res = str_accumulate("Result of ");
    
    if(strcmp((char *)arg, "first") == 0)  // for test!
        sleep(3);

    res = str_accumulate((char *)arg);
    res = str_accumulate(" thread");

    printf("thread %lx: \"%s\"\n", pthread_self(), res);
    return NULL;
}

int main(int argc, char *argv[])
{
    int ret_val;
    
    char * res;
    pthread_t th1, th2;
    
    res = str_accumulate("Result of ");
    pthread_create(&th1, NULL, thread_func,(void *)"first");
    pthread_create(&th2, NULL, thread_func,(void *)"second");

    
    res = str_accumulate("main thread");
    printf("thread %lx: \"%s\"\n", pthread_self(), res);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    pthread_exit(0);
    return 0;

}


