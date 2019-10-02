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



#define SERV_FIFO   "/tmp/fifo.serv"
#define FIFO1   "/tmp/fifo.1"
#define FIFO2   "/tmp/fifo.2"
#ifndef MAXLINE
#define MAXLINE 1024
#endif
#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void    server(int, int);
static ssize_t my_read(int fd, char *ptr);
ssize_t readline(int fd, void *vptr, size_t maxlen);
ssize_t Readline(int fd, void *ptr, size_t maxlen);


static ssize_t my_read(int fd, char *ptr)
{
    static int  read_cnt = 0;
    static char *read_ptr;
    static char read_buf[MAXLINE];

    if (read_cnt <= 0) {
again:
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return(-1);
        } else if (read_cnt == 0)
            return(0);
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return(1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    int     n, rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ( (rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;  /* newline is stored, like fgets() */
        } else if (rc == 0) {
            if (n == 1)
                return(0);  /* EOF, no data read */
            else
                break;      /* EOF, some data was read */
        } else
            return(-1);     /* error, errno set by read() */
    }

    *ptr = 0;   /* null terminate like fgets() */
    return(n);
}
/* end readline */

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
    ssize_t     n;

    if ( (n = readline(fd, ptr, maxlen)) < 0)
    {
        printf("readline error, will exit\n");
        exit(1);
    }
        
    return(n);
}

int main(int argc, char **argv)
{
    int     readfifo, writefifo, dummyfd, fd;
    char    *ptr, buff[MAXLINE], fifoname[MAXLINE];
    pid_t   pid;
    ssize_t n;

        /* 4create server's well-known FIFO; OK if already exists */
    if ((mkfifo(SERV_FIFO, FILE_MODE) < 0) && (errno != EEXIST))
        printf("can't create %s\n", SERV_FIFO);

        /* 4open server's well-known FIFO for reading and writing */
    if((readfifo = open(SERV_FIFO, O_RDONLY, 0)) < 0)
        perror("open for read failed");
    if((dummyfd = open(SERV_FIFO, O_WRONLY, 0)) < 0)        /* never used */
        perror("open for write failed");
        
    while ((n = Readline(readfifo, buff, MAXLINE)) > 0) {
        if (buff[n-1] == '\n')
            n--;            /* delete newline from readline() */
        buff[n] = '\0';     /* null terminate pathname */

        if ( (ptr = strchr(buff, ' ')) == NULL) {
            perror("bogus request");
            continue;
        }

        *ptr++ = 0;         /* null terminate PID, ptr = pathname */
        pid = atol(buff);
        snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long) pid);
        if ((writefifo = open(fifoname, O_WRONLY, 0)) < 0) {
            perror("cannot open fifoname");
            continue;
        }

        if ((fd = open(ptr, O_RDONLY)) < 0) {
                /* 4error: must tell client */
            snprintf(buff + n, sizeof(buff) - n, ": can't open, %s\n",
                     strerror(errno));
            n = strlen(ptr);
            if(write(writefifo, ptr, n) != n)
            {
                printf("write error, n:%lu, will exit\n", n);
                exit(1);
            }
            close(writefifo);
    
        } else {
                /* 4open succeeded: copy file to FIFO */
            while ((n = read(fd, buff, MAXLINE)) > 0)
                if(write(writefifo, buff, n) != n)
                {
                    printf("write error, n:%lu, will exit\n", n);
                    exit(1);
                }
            close(fd);
            close(writefifo);
        }
    }
    
    return 0;
}
