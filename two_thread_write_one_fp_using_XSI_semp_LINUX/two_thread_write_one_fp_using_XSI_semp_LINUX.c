#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>


//#include <sys/ipc.h>
#include <linux/ipc.h>
//#include <sys/sem.h>
#include <linux/sem.h>

#include <sys/types.h>
// #include <linux/types.h>

#include <sys/syscall.h>

typedef signed  int                             SINT32;
typedef unsigned  int                           UINT32;
typedef unsigned short                          UINT16;

/* declaration */
int semop(int semid, struct sembuf *sops, size_t nsops);
int semctl(int semid, int semnum, int cmd, union semun argument);
int semget(key_t key, int nsems, int semflg);

int semop(int semid, struct sembuf *sops, size_t nsops)
{
    return syscall(__NR_semop, semid, sops, nsops);
}

int semctl(int semid, int semnum, int cmd, union semun argument)
{
    return syscall(__NR_semctl, semid, semnum, cmd, argument);
}

int semget(key_t key, int nsems, int semflg)
{
    return syscall(__NR_semget, key, nsems, semflg);
}


FILE * fp1, * fp2;
#define FILE_NAME  "test222"

#if 0
union semun {               /* define union for semctl() */
  int              val;
  struct semid_ds *buf;
  unsigned short  *array;
};
#endif

#define SVSEM_MODE  (0755) /* default permissions for new SV semaphores */
// #define  SVSEM_MODE  (SEM_R | SEM_A | SEM_R>>3 | SEM_R>>6)

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
    return NULL;
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
    return NULL;
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
