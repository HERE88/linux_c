

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


#include <sys/time.h>



void maketimeout(struct timespec *tsp, long minutes)
{
    struct timeval now;

    //get curreent time
    gettimeofday(&now, NULL);
    tsp->tv_sec = now.tv_sec;
    tsp->tv_nsec = now.tv_usec * 1000;  //usec to nsec

    //add the offset to get timeout value
    tsp->tv_sec += minutes * 60;

}

pthread_cond_t  rsp_ready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t rsp_lock  = PTHREAD_MUTEX_INITIALIZER;
int wait_rsp_status = 0;  // 0: waiting, 1: completed

int main(int argc, char **argv)
{
    int ret_val;
    struct timespec tsp;
    maketimeout(&tsp, 1);

    pthread_mutex_lock(&rsp_lock);
    while(0 == wait_rsp_status)
    {
        printf("cond sleeping...\n");
        ret_val = pthread_cond_timedwait(&rsp_ready, &rsp_lock, &tsp);

        if(ret_val == ETIMEDOUT)
        {
            printf("ETIMEOUT recvd, will break! \n");
        }
        break;

    }

    printf("cond sleep end \n");
    pthread_mutex_unlock(&rsp_lock);



    return 0;


}
