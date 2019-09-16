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


volatile sig_atomic_t isGpsReady;

static void sigUsr2(int signo)
{
    if(signo == SIGUSR2)
    {
        isGpsReady = 1;
    }

}

int main(int argc, char *argv[])
{
    sigset_t newmask, old_mask, zeromask;
    if(signal(SIGUSR2, sigUsr2) == SIG_ERR)
    {
        printf("signal(SIGUSR2) error.\n");
    }

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR2);

    if(sigprocmask(SIG_BLOCK, &newmask, &old_mask) < 0)
    {
        printf("SIG_BLOCK error.\n");
    }

    printf("getpid= %d\n", getpid());

    while(isGpsReady == 0)
    {
        sigsuspend(&zeromask);
    }
    isGpsReady = 0;

    if(sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0)
    {
        printf("SIG_SETMASK error.\n");
    }

    printf("process will continue...\n");

    return 0;
}