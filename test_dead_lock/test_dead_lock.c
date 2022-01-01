#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

static int sequence1 = 0;
static int sequence2 = 0;

pthread_mutex_t lock1;
pthread_mutex_t lock2;

int func1()
{
    pthread_mutex_lock(&lock1); 
    ++sequence1; 
    usleep(1000*1000); 
    pthread_mutex_lock(&lock2); 
    ++sequence2; 
    pthread_mutex_unlock(&lock2); 
    pthread_mutex_unlock(&lock1); 

    return sequence1; 
}

int func2()
{
    pthread_mutex_lock(&lock2); 
    ++sequence2; 
    usleep(1000*1000); 
    pthread_mutex_lock(&lock1); 
    ++sequence2; 
    pthread_mutex_unlock(&lock1); 
    pthread_mutex_unlock(&lock2); 

    return sequence1; 
}


void* thread1(void *arg)
{
    while(1)
    {
        if(func1() == 100000)
        {
            pthread_exit(NULL);
        }
    }
}

void* thread2(void *arg)
{
    while(1)
    {
        if(func2() == 100000)
        {
            pthread_exit(NULL);
        }
    }
}

void* thread3(void *arg)
{
    int count = 10000;
    while(count--)
        usleep(1000*1000);

    pthread_exit(NULL);
}

void* thread4(void *arg)
{
    int count = 10000;
    while(count--)
        usleep(1000*1000);

    pthread_exit(NULL);
}



int main()
{
    pthread_t tid[4];
    
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);


    if(pthread_create(&tid[0], NULL, &thread1, NULL) != 0)
    {
        _exit(1);
    }

    if(pthread_create(&tid[1], NULL, &thread2, NULL) != 0)
    {
        _exit(1);
    }

    if(pthread_create(&tid[2], NULL, &thread3, NULL) != 0)
    {
        _exit(1);
    }

    if(pthread_create(&tid[3], NULL, &thread4, NULL) != 0)
    {
        _exit(1);
    }

    usleep(5*1000*1000); 


    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);


    pthread_mutex_destroy( &lock1 );
    pthread_mutex_destroy( &lock2 );

    return 0;
}