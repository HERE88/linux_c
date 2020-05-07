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

#include <math.h>

#include <mqueue.h>

//#include "apue.h"
#include <pthread.h>

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

//////////////////////////////////////////////
int my_lock(int fd);
int my_unlock(int fd);
static void signal_treatment(int signo);
static int handle_signals(void);


////////////////////////////////////////////////////

int my_lock(int fd)
{
    int ret;
    struct flock    lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;             /* write lock entire file */

    ret = fcntl(fd, F_SETLK, &lock);
    if(ret == -1)
    {
        printf("my_lock: fcntl error\n");
        return -1;
    }
    return 0;
}

int my_unlock(int fd)
{
    int ret;
    struct flock    lock;

    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;             /* unlock entire file */

    ret = fcntl(fd, F_SETLK, &lock);
    if(ret == -1)
    {
        printf("my_unlock: fcntl error\n");
        return -1;
    }
    return 0;
}

static void signal_treatment(int signo)
{
    int  ret_val = 0;

    printf("receive signal: %d. \n", signo);

    switch (signo)
    {
        case SIGINT:

        case SIGTERM:

        case SIGHUP:
            //server_stop();
            printf("The process will exit by signal! \n");
            exit(0);
            break;

        case SIGUSR1:
            printf("thread %u will be end. \n", (unsigned int)pthread_self());
            pthread_exit(NULL);
            break;

        default:
            printf("unkonwn signal, don't treat! \n");
            break;
    }
}
static int handle_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = signal_treatment;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGHUP);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sa.sa_flags = 0;

    if(sigaction(SIGINT, &sa, NULL) < 0) {
        printf("can't catch SIGINT: %s \n", strerror(errno));
        exit(1);
    }

    if(sigaction(SIGTERM, &sa, NULL) < 0) {
        printf("can't catch SIGTERM: %s \n", strerror(errno));
        exit(1);
    }

    if(sigaction(SIGHUP, &sa, NULL) < 0) {
        printf("can't catch SIGHUP: %s \n", strerror(errno));
        exit(1);
    }

    if(sigaction(SIGUSR1, &sa, NULL) < 0) {
        printf("can't catch SIGUSR1: %s \n", strerror(errno));
        exit(1);
    }

    return(0);
}
int main()
{
    int fd, ret;

    handle_signals();

    fd = open("test1.data", O_RDWR | O_CREAT, FILE_MODE);
    if(fd < 0)
    {
        printf("open file error.\n");
        return -1;
    }

    ret = my_lock(fd);
    if(ret != 0)
    {
        printf("my_lock error, will exit\n");
        return 0;
    }

    printf("my_lock success\n");
    while(1)
    {
        sleep(0xffffffff);
    }

    return 0;
}