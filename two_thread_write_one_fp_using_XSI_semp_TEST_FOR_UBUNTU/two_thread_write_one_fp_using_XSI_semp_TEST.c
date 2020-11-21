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

#include <math.h>

#include <mqueue.h>

//#include "apue.h"
#include <pthread.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

typedef signed  int                             SINT32;
typedef unsigned  int                           UINT32;
typedef unsigned short                          UINT16;

FILE * fp1, * fp2;

#define FILE_NAME  "test111"

union semun {				/* define union for semctl() */
  int              val;
  struct semid_ds *buf;
  unsigned short  *array;
};


#define	SVSEM_MODE	(0755) /* default permissions for new SV semaphores */
int g_semid = -1;

void * th1(void * param)
{
    int i;
    char buf[1024];
    struct sembuf tmp;

    printf("th1 enter\n");

    for(i = 0; i < 10000000; ++i)
    {
        snprintf(buf, sizeof(buf), "%08dwang\n", i);
        fwrite(buf, 1, strlen(buf),fp1);
    }

    printf("will semop_th1...\n");
    tmp.sem_num = 0;
    tmp.sem_op = -1;
    tmp.sem_flg = 0;
    if(semop(g_semid, &tmp, 1) == -1)
    {
        printf("semop_th1 error:%d. %s\n", errno, strerror(errno));
    }

    printf("th1 end\n");
}


void * th2(void * param)
{
    struct sembuf tmp;

    printf("th2 enter\n");
    usleep(100*1000);
    fseek(fp2, 8, SEEK_SET);

    fwrite("WWWW", 1, strlen("WWWW"), fp2);
    //fflush(fp2);
    fclose(fp2);

    printf("will semop_th2...\n");
    tmp.sem_num = 0;
    tmp.sem_op = -1;
    tmp.sem_flg = 0;
    if(semop(g_semid, &tmp, 1) == -1)
    {
        printf("semop_th2 error:%d. %s\n", errno, strerror(errno));
    }

    printf("th2 end\n");
}


int main(int argc, char * argv[])
{
    int ret;
    pthread_t tid1, tid2;
    
    union semun arg;

    struct sembuf * ptr;

    fp1 = fopen(FILE_NAME, "wb+");
    fp2 = fopen(FILE_NAME, "rb+");
    assert(fp1 != NULL && fp2 != NULL);

    g_semid = semget(IPC_PRIVATE, 1, SVSEM_MODE | IPC_CREAT | IPC_EXCL);
    if(g_semid < 0)
    {
        printf("semget error:%d. %s\n", errno, strerror(errno));
        return -1;
    }

    arg.val = 0;  //initialize to 0
    ret = semctl(g_semid, 0, SETVAL, arg);
    if(ret == -1)
    {
        printf("semctl error:%d. %s\n", errno, strerror(errno));
        return -1;
    }

    ptr = calloc(1, sizeof(struct sembuf));
    if(ptr == NULL)
    {
        printf("calloc error:%d. %s\n", errno, strerror(errno));
        return -1;
    }

    ptr->sem_num = 0;
    ptr->sem_op = 1;
    ptr->sem_flg = 0;
    if(semop(g_semid, ptr, 1) == -1)
    {
        printf("semop_main_1 error:%d. %s\n", errno, strerror(errno));
    }
    pthread_create(&tid1, NULL, th1, NULL);


    ptr->sem_num = 0;
    ptr->sem_op = 1;
    ptr->sem_flg = 0;
    if(semop(g_semid, ptr, 1) == -1)
    {
        printf("semop_main_2 error:%d. %s\n", errno, strerror(errno));
    }
    pthread_create(&tid2, NULL, th2, NULL);

    // printf("sleep 10s begin...\n");
    // sleep(10);
    // printf("sleep 10s end.\n");

    printf("waiting for all thread complete....\n");
    ptr->sem_num = 0;
    ptr->sem_op = 0;
    ptr->sem_flg = 0;
    if(semop(g_semid, ptr, 1) == -1)
        printf("semop_main_3 error:%d. %s\n", errno, strerror(errno));
    else printf("semop_main_3 success.\n");

    //fflush(fp1);
    fclose(fp1);

    free(ptr);

    return 0;
}
