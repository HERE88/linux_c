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
    int wait_time;
    //int dev_fd;
    
}DataStatRel;

DataStatRel dataStat;
int dev_fd;
int write_len;
char buf[1024];

#define WRITE_LENGTH  256


/* declaration */
typedef void (*timerTimeout)(union sigval sig);
void createTimer(DataStatRel * dsPtr, timerTimeout func);
void setTimer(DataStatRel * dsPtr, TIMER_TYPE type);
void deleteTimer(timer_t *timerId);

char *getTimeStr(char ** bufPtr)
{
    char msgBuf[64] = {0};
    struct timeval tv;
    struct tm * tm_ptr;

    gettimeofday(&tv, NULL);
    tm_ptr = localtime(&(tv.tv_sec));

    /*snprintf(msgBuf, sizeof(msgBuf),"%02d-%02d %02d:%02d:%02d.%06d", (tm_ptr->tm_mon + 1), tm_ptr->tm_mday,
        tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, tv.tv_usec);*/
    snprintf(msgBuf, sizeof(msgBuf),"%02d:%02d:%02d.%06d",(int)tm_ptr->tm_hour, (int)tm_ptr->tm_min,
        (int)tm_ptr->tm_sec, (int)tv.tv_usec);

    //printf("msgBuf: %s\n", msgBuf);
    strncpy(*bufPtr, msgBuf, sizeof(msgBuf));

    return (*bufPtr);
}


void dataTimeoutThread(union sigval sig)

{
    
    // Buffer to print logs!
    char msgBuf[1024] = {0};
    char timeStr[64] = {0};
    char *tPtr = timeStr;
	dataStat.wait_time = (random() % 10 + 1)*1000;

    printf("[%s]set dataStat.wait_time = [%d]\n", getTimeStr(&tPtr), dataStat.wait_time);

    
	setTimer(&dataStat, TIMER_ONE_SHOT);

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
    printf("createTimer success, timerId[%ld] \n", (long)(*timerId));
    
    return;  
}

void setTimer(DataStatRel * dsPtr, TIMER_TYPE type)  
{  
    struct itimerspec its;  
    timer_t *timerId = &(dsPtr->timerId);
    int timeMSec = dsPtr->wait_time;
  
    if (*timerId == -1)
    {
        printf("invalid parameter.timerId:%ld; timeMSec:%d. \n", (long)(*timerId), timeMSec);
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
    printf("setTimer success, timerId[%ld], timeMSec:%u. \n", (long)(*timerId), timeMSec);

    return;  
}  

void deleteTimer(timer_t *timerId)  
{  
  if (*timerId != -1)  
  {  
    printf("delete timer.timerId:%ld. \n", (long)(*timerId));
    timer_delete(*timerId);  
    *timerId = -1;  
  }  

  printf("deleteTimer success, timerId[%ld] \n", (long)(*timerId));
  
  return;  
}

int open_device(const char *device_name, int mode, int retry_count)
{
    int i = 1;
    int fd = -1;

    if (device_name == NULL)
        return -1;

    do {
        if(0 == access(device_name, F_OK)) {
            fd = open(device_name, mode);
            if(fd > 0) {
                break;
            } else {
                printf("open device %s failed, tried %d (%s)\n", device_name, i, strerror(errno));
            }
        } else {
            printf("access device %s failed, tried %d (%s)\n", device_name, i, strerror(errno));
        }

        usleep(1000*1000);  //sleep 1s.
        i++;
    } while(i <= retry_count);

    return fd;
}

int main(int argc, char *argv[])
{
    int i = 0;



	//Initialize
	memset(&dataStat, 0, sizeof(DataStatRel));
	dataStat.timerId = -1;
	dataStat.wait_time = (random() % 10 + 1)*1000;

	createTimer(&dataStat, dataTimeoutThread);
	setTimer(&dataStat, TIMER_ONE_SHOT); //若为TIMER_PERIODIC，由dataTimeoutThread中不能deleteTimer

	while(1)
	{
		sleep(0xffffffff);
	}; //等待定时器，否则主进程会直接退出
    
    return 0;
}

