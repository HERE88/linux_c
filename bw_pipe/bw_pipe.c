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

#include <sys/time.h>



void    reader(int, int, int);
void    writer(int, int);
ssize_t Read(int fd, void *ptr, size_t nbytes);
void    Write(int fd, void *ptr, size_t nbytes);


void    err_sys(const char *fmt, ...);
void    err_quit(char *ptr);
static void    err_doit(int errnoflag, int level, const char *fmt, va_list ap);


int     start_time(void);
double  stop_time(void);
int     touch(void *vptr, int nbytes);
void    tv_sub(struct timeval *out, struct timeval *in);


void    Start_time(void);
double  Stop_time(void);
void    Touch(void *vptr, int nbytes);
void *  Valloc(size_t size);
pid_t   Fork(void);
void    Pipe(int *fds);


#define MAXLINE     4096    /* max text line length */

void    *buf;
int     totalnbytes, xfersize;

static struct timeval   tv_start, tv_stop;


void
err_sys(const char *fmt, ...)
{
    va_list     ap;

    va_start(ap, fmt);
    err_doit(1, 0, fmt, ap);
    va_end(ap);
    exit(1);
}

void
err_quit(char *ptr)
{
    fprintf(stderr, "%s\n", ptr);
    exit(1);
}

static void
err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
    int     errno_save, n;
    char    buf[MAXLINE];

    errno_save = errno;     /* value caller might want printed */
#ifdef  HAVE_VSNPRINTF
    vsnprintf(buf, sizeof(buf), fmt, ap);   /* this is safe */
#else
    vsprintf(buf, fmt, ap);                 /* this is not safe */
#endif
    n = strlen(buf);
    if (errnoflag)
        snprintf(buf+n, sizeof(buf)-n, ": %s", strerror(errno_save));
    strcat(buf, "\n");

        fflush(stdout);     /* in case stdout and stderr are the same */
        fputs(buf, stderr);
        fflush(stderr);

    return;
}


void *
Valloc(size_t size)
{
    void    *ptr;

#ifdef  HAVE_VALLOC
    if ( (ptr = valloc(size)) == NULL)
        err_sys("valloc error");
#else
    if ( (ptr = malloc(size)) == NULL)
        err_sys("malloc error (Valloc)");
#endif
    return(ptr);
}

pid_t
Fork(void)
{
    pid_t   pid;

    if ( (pid = fork()) == -1)
        err_sys("fork error");
    return(pid);
}

void
Pipe(int *fds)
{
    if (pipe(fds) < 0)
        err_sys("pipe error");
}


int
start_time(void)
{
    return(gettimeofday(&tv_start, NULL));
}

double
stop_time(void)
{
    double  clockus;

    if (gettimeofday(&tv_stop, NULL) == -1)
        return(0.0);
    tv_sub(&tv_stop, &tv_start);
    clockus = tv_stop.tv_sec * 1000000.0 + tv_stop.tv_usec;
    return(clockus);
}

int
touch(void *vptr, int nbytes)
{
    char    *cptr;
    static int  pagesize = 0;

    if (pagesize == 0) {
        errno = 0;
#ifdef  _SC_PAGESIZE
        if ( (pagesize = sysconf(_SC_PAGESIZE)) == -1)
            return(-1);
#else
        pagesize = getpagesize();       /* BSD */
#endif
    }

    cptr = vptr;
    while (nbytes > 0) {
        *cptr = 1;
        cptr += pagesize;
        nbytes -= pagesize;
    }
    return(0);
}
/* end start_time */

void
Start_time(void)
{
    if (start_time() == -1)
        err_sys("start_time error");
}

double
Stop_time(void)
{
    double  clockus;

    if ( (clockus = stop_time()) == 0.0)
        err_sys("stop_time error");
    return(clockus);
}

void
Touch(void *vptr, int nbytes)
{
    if (touch(vptr, nbytes) == -1)
        err_sys("touch error");
}

void
tv_sub(struct timeval *out, struct timeval *in)
{
    if ( (out->tv_usec -= in->tv_usec) < 0) {   /* out -= in */
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

void
writer(int contfd, int datafd)
{
    int     ntowrite;

    for ( ; ; ) {
        Read(contfd, &ntowrite, sizeof(ntowrite));

        while (ntowrite > 0) {
            Write(datafd, buf, xfersize);
            ntowrite -= xfersize;
        }
    }
}

void
reader(int contfd, int datafd, int nbytes)
{
    ssize_t n;

    Write(contfd, &nbytes, sizeof(nbytes));

    while ((nbytes > 0) &&
           ( (n = Read(datafd, buf, xfersize)) > 0)) {
        nbytes -= n;
    }
}

ssize_t
Read(int fd, void *ptr, size_t nbytes)
{
    ssize_t     n;

    if ( (n = read(fd, ptr, nbytes)) == -1)
        err_sys("read error");
    return(n);
}

void
Write(int fd, void *ptr, size_t nbytes)
{
    if (write(fd, ptr, nbytes) != nbytes)
        err_sys("write error");
}




int
main(int argc, char **argv)
{
    int     i, nloop, contpipe[2], datapipe[2];
    pid_t   childpid;

    if (argc != 4)
        err_quit("usage: bw_pipe <#loops> <#mbytes> <#bytes/write>");
    nloop = atoi(argv[1]);
    totalnbytes = atoi(argv[2]) * 1024 * 1024;
    xfersize = atoi(argv[3]);

    buf = Valloc(xfersize);
    Touch(buf, xfersize);

    Pipe(contpipe);
    Pipe(datapipe);

    if ( (childpid = Fork()) == 0) {
        writer(contpipe[0], datapipe[1]);   /* child */
        exit(0);
    }
        /* 4parent */
    Start_time();
    for (i = 0; i < nloop; i++)
        reader(contpipe[1], datapipe[0], totalnbytes);
    printf("bandwidth: %.3f MB/sec\n",
           totalnbytes / Stop_time() * nloop);
    kill(childpid, SIGTERM);
    exit(0);
}

