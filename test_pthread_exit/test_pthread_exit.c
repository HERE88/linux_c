#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 

void *myfunc1() 
{ 
    float *ret = malloc(sizeof *ret); 
    *ret = 3.14; 
    // code segments 
    pthread_exit(ret);    
} 


void *myfunc2() 
{ 
    float *ret = malloc(sizeof *ret); 
    *ret = 3.15; 
    // code segments 
    pthread_exit(ret);    
} 

int main(int argc, char *argv[]) 
{ 

    void *sum1, *sum2; 
    pthread_t thread_id1,thread_id2; 

    pthread_create(&thread_id1,NULL,myfunc1,NULL); 
    pthread_create(&thread_id2,NULL,myfunc2,NULL); 
    
    printf("after pthread_create\n"); 
    pthread_join(thread_id1, &sum2); 
    pthread_join(thread_id2, &sum1); 
    
    printf("after pthread_join\n"); 
    float firstSum = *((float *)sum1); 
    float secondSum = *((float *)sum2); 

    printf("Sum1 is %.5f\n", firstSum); 
    printf("Sum2 is %.5f\n", secondSum); 
    free(sum1);
    free(sum2);
    
    return 0; 
} 

