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

typedef signed  int                             SINT32;
typedef unsigned  int                           UINT32;

SINT32 findPidByName(char* procName, int* foundPid, int idCount);
SINT32 isProcessRunning(char *procName, int *pidArr, int itemCnt);


SINT32 findPidByName(char* procName, int* foundPid, int idCount)
{
    DIR *dir;
    struct dirent *d_ptr;
    int pid_temp, i = 0;

    FILE *status_file;
    char filename_temp[128];
    char buffer[256];
    char name[128];

    if((!procName) || (strlen(procName) == 0) || (idCount < 2))
    {
        printf("findPidByName invalid parameter. \n");
        return -1;
    }

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if(!dir)
    {
        printf("cannot open /proc\n");
        return -1;
    }

    /* Walk through the directory. */
    while((d_ptr = readdir(dir)) != NULL)
    {
        /* Must skip ".." since that is outside /proc */
        if(strcmp(d_ptr->d_name, "..") == 0 || strcmp(d_ptr->d_name, ".") == 0)
            continue;

        //if not a directory, continue
        if(DT_DIR != d_ptr->d_type)
            continue;

        /* See if this is a process */
        if((pid_temp = atoi(d_ptr->d_name)) == 0) //atoi return 0 if failed
            continue;

        //is a process's pid
        memset(filename_temp, 0 ,sizeof(filename_temp));
        sprintf(filename_temp, "/proc/%s/status", d_ptr->d_name);

        if(!(status_file = fopen(filename_temp, "r")) )
            continue;

        if(fgets(buffer, sizeof(buffer) - 1, status_file) == NULL) //read a line to buffer
        {
            fclose(status_file);
            continue;
        }

        fclose(status_file);

        //get pids of processes
        /* Buffer should contain a string like "Name:   binary_name" */
        sscanf(buffer, "%*s %s", name);
        if(strcmp(name, procName) == 0)   //match the process's name
        {
            foundPid[i] = pid_temp; //store pid number in pid's array
            //printf("foundPid[i] = %d\n", foundPid[i]);
            i++;
            if(i >= (idCount - 1)) //idCount is the max num of searches
            {
                printf("search num has reached its limit\n");
                break;
            }
        }
    }
    closedir(dir);

    return  0;
}

SINT32 isProcessRunning(char *procName, int *pidArr, int itemCnt)
{
    int rtn = 0;
    int i = 0;
    int pidTemp = 0;

    if((!procName) || (strlen(procName) == 0))
    {
        printf("isProcessRunning invalid parameter. \n");
        return -1;
    }

    memset(pidArr, 0, itemCnt*sizeof(int));  //initialize pid arrsy to zero

    //get pid
    rtn = findPidByName(procName, pidArr, itemCnt);
    if(rtn != 0)
    {
        printf("cal find_pid_by_name, return error. \n") ;  //if find error, return -1
        return 1;
    }

    //print the pid of each processes found
    for(i = 0; pidArr[i] != 0; ++i)
    {
        pidTemp = pidArr[i];
        printf("process(%s) pid is: %d, i:%d. \n", procName, pidTemp, i);

        if(pidTemp <= 0)
        {
            printf("getted pid(%d) is not valid. \n", pidTemp);
            return -1;
        }
    }

    if(i == 0)
        return -1;
    else 
        return  0; //return success
}

int main()
{
    int pid_t[32]; //default max num of searches is: 32
    int ret, i;
    memset(pid_t, 0, sizeof(pid_t));

    ret = isProcessRunning("qwer1234", pid_t, sizeof(pid_t)/sizeof(int));
    if(ret == 0)
    {
        printf("qwer1234 is running, elog will exit!\n");
        exit(0);
    }
    else printf("wang!\n");

    return 0;
}