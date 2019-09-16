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


int main(int argc, char *argv[])
{
    pid_t pid;

    if(argc != 2)
    {
        printf("param error!\n");
        return 0;
    }

    pid = atoi(argv[1]);
    printf("pid =%d\n", pid);

    if(0 != kill(pid, SIGUSR2))
    {
        printf("kill failed\n");
    }
    else
    {
        printf("kill SIGUSR2 success.\n");
    }

    return 0;
}