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

//add for POSIX timer
typedef enum
{
    TIMER_MIN = 0,
    TIMER_ONE_SHOT,
    TIMER_PERIODIC,
    TIMER_MAX
} TIMER_TYPE;

typedef struct DataStatRelSt
{
    timer_t timerId;
    int test;
    int wait_time;

}DataStatRel;


typedef void (*timerTimeout)(union sigval sig);
void createTimer(DataStatRel * dsPtr, timerTimeout func);
void setTimer(DataStatRel * dsPtr, TIMER_TYPE type);
void deleteTimer(timer_t *timerId);


void dataTimeoutThread(union sigval sig)

{
    int temp = 0;
    DataStatRel * dsTmpPtr = (DataStatRel *)sig.sival_ptr;
    printf("start dataTimeoutThread! \n");

    temp = dsTmpPtr->test;
    printf("wanghaitao success! temp = %d\n",temp);

    deleteTimer(&(dsTmpPtr->timerId));
}

//add for POSIX timer
void createTimer(DataStatRel * dsPtr, timerTimeout func)
{
    struct sigevent sev;
    timer_t *timerId = &(dsPtr->timerId);

    memset(&sev, 0, sizeof(struct sigevent));
    sev.sigev_notify = SIGEV_THREAD;    //start function of a new thread
    //sev.sigev_signo = SIGRTMIN;

    sev.sigev_value.sival_ptr = dsPtr;  //传递给sigev_notify_function中的函数

    sev.sigev_notify_function = func;    //Function used for thread notification (SIGEV_THREAD)
    sev.sigev_notify_attributes = NULL;

    if (*timerId != -1)
    {
       deleteTimer(timerId);
    }

    /* create timer */
    if (timer_create(CLOCK_REALTIME, &sev, timerId) == -1)
    {
        printf("timer_create, error: %d - %s \n",  errno, strerror(errno));
        return;
    }

    if (*timerId == -1)
    {
        printf("timer_create, error, id is -1.\n");
        return;
    }
    printf("createTimer success, timerId[%d] \n", *timerId);

    return;
}

void setTimer(DataStatRel * dsPtr, TIMER_TYPE type)
{
    struct itimerspec its;
    timer_t *timerId = &(dsPtr->timerId);
    int timeMSec = dsPtr->wait_time;

    if (*timerId == -1)
    {
        printf("invalid parameter.timerId:%d; timeMSec:%d. \n", *timerId, timeMSec);
        return;
    }

    /* Start the timer */
    its.it_value.tv_sec =  timeMSec / 1000;
    its.it_value.tv_nsec = (timeMSec % 1000) * 1000000;

    if (type == TIMER_PERIODIC)
    {
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
    }
    else
    {
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;
    }

    if (timer_settime(*timerId, 0, &its, 0) == -1)
    {
        printf("timer_settime errorr: %d - %s \n",  errno, strerror(errno));
        return;
    }
    printf("setTimer success, timerId[%d], timeMSec:%u. \n", *timerId, timeMSec);

    return;
}

void deleteTimer(timer_t *timerId)
{
    if (*timerId != -1)
    {
        printf("delete timer.timerId:%d. \n", *timerId);
        timer_delete(*timerId);
        *timerId = -1;
    }

    printf("deleteTimer success, timerId[%d] \n", *timerId);

    return;
}

DataStatRel dataStat;


int main()
{
    memset(&dataStat, 0, sizeof(DataStatRel));
    dataStat.test = 9011;
    dataStat.timerId = -1;
    dataStat.wait_time = 10*1000; //10s

    createTimer(&dataStat, dataTimeoutThread);
    setTimer(&dataStat, TIMER_ONE_SHOT);

    while(1); //等待定时器，否则主进程会直接退出

    return 0;
}

