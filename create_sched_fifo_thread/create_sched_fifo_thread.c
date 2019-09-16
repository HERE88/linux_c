#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>


pthread_t createSchedFifoThread(void* (*pThreadFunc)(void*), int priority)
{
        struct sched_param param;
        int policy;
        int ret;
        pthread_t id = 0;

        pthread_attr_t thread_attr;
        ret = pthread_attr_init(&thread_attr);
        if (0 != ret) {
                printf("Attribute creation failed");
                return id;
        }

        ret = pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
        if (0 != ret) {
                printf("pthread_attr_setschedpolicy failed");
                return id;
        }

        param.sched_priority = priority;

        ret = pthread_attr_setschedparam(&thread_attr, &param);
        if (0 != ret) {
                printf("Setting pthread_attr_setschedparam failed");
                return id;
        }

        ret = pthread_create(&id, &thread_attr, pThreadFunc, NULL);
        if (0 != ret) {
                printf("Thread creation failed");
                return id;
        }

        pthread_attr_destroy(&thread_attr);
        return id;
}

void* thread1(void *arg)
{
        int ret = -1;
        int policy;
        struct sched_param param;
        ret = pthread_getschedparam(pthread_self(), &policy, &param);
        if(ret != 0)
            perror("pthread_getschedparam error");
        switch(policy)
        {
            case SCHED_FIFO:
                printf("SCHED_FIFO tp\n");
                break;
            case SCHED_RR:
                printf("SCHED_FIFO tp\n");
                break;
            case SCHED_OTHER:
                printf("SCHED_OTHER tp\n");
                break;


        }
        while(1)
        {
                //printf("thread1\n");
        }
}

void* thread2(void *arg)
{       int ret = -1;
        int policy;
        struct sched_param param;

        ret = pthread_getschedparam(pthread_self(), &policy, &param);
        if(ret != 0)
            perror("pthread_getschedparam error");

        switch(policy)
        {
            case SCHED_FIFO:
                printf("SCHED_FIFO tp\n");
                break;
            case SCHED_RR:
                printf("SCHED_FIFO tp\n");
                break;
            case SCHED_OTHER:
                printf("SCHED_OTHER tp\n");
                break;
        }
        while(1)
        {
                //printf("thread2\n");
        }
}

int main(void)
{
        pthread_t id;
        printf("create thread1\n");
        //pthread_create(&id, NULL, thread1,NULL);
        id = createSchedFifoThread(thread1, 10);
        sleep(3);
        printf("create thread2\n");
        id = createSchedFifoThread(thread2, 30);
        while(1)
        {
                sleep(1);
        }
        return 0;
}
