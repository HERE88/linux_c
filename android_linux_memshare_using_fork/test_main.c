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

#include <sys/mman.h>


#define SHARED_REGION "shared_region_for_gps"


int main()
{
    int ret_val = 0;
    size_t region_size = 0;

    pid_t pid;

    int fd;
    char *data_ptr = NULL;

    region_size = 32;

    fd = ashmem_create_region(SHARED_REGION,region_size);
    if(fd < 0)
    {
        printf("ashmem_create_region error\n");
    }

    if((pid = fork()) < 0)
    {
        printf("fork error\n");
    }
    else if(pid == 0)  //child
    {
        data_ptr = mmap(NULL, region_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if(data_ptr  != MAP_FAILED)
        {
            printf("child mmap sharemem success\n");
            sleep(5);
            printf("child data_ptr = %08x\n", *((int *)data_ptr));

        }
        else
        {
            printf("mmap sharemem failed....'%s'\n", strerror(errno));
            close(fd);
        }

        close(fd);
        return ret_val;
    }

    //parent
    data_ptr = mmap(NULL, region_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(data_ptr  != MAP_FAILED)
    {
        printf("mmap sharemem success\n");
        memset(data_ptr, 0xfe, region_size);

    }
    else
    {
        printf("mmap sharemem failed....'%s'\n", strerror(errno));
        close(fd);
    }

    while(1);
    return ret_val;

}

