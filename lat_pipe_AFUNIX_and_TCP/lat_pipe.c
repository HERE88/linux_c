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


//add 20210927:
#include <sys/types.h>
#include <netdb.h>


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

void
Kill(pid_t pid, int signo)
{
	if (kill(pid, signo) == -1)
		err_sys("kill error");
}


/*
 * Create a server endpoint of a connection.
 * Returns fd if all OK, <0 on error.
 */
int
serv_listen(const char *name)
{
    int                 fd, len, err, rval;
    struct sockaddr_un  un;

    if (strlen(name) >= sizeof(un.sun_path)) {
        errno = ENAMETOOLONG;
        return(-1);
    }

    /* create a UNIX domain stream socket */
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        return(-2);

    unlink(name);   /* in case it already exists */

    /* fill in socket address structure */
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(name);

    /* bind the name to the descriptor */
    if (bind(fd, (struct sockaddr *)&un, len) < 0) {
        rval = -3;
        goto errout;
    }

    if (listen(fd, 10) < 0) {   /* tell kernel we're a server */
        rval = -4;
        goto errout;
    }
    return(fd);

errout:
    err = errno;
    close(fd);
    errno = err;
    return(rval);
}


/*
 * Create a client endpoint and connect to a server.
 * Returns fd if all OK, <0 on error.
 */
int
cli_conn(const char *name)
{
    int                 fd, len, err, rval;
    struct sockaddr_un  un, sun;
    int                 do_unlink = 0;

    if (strlen(name) >= sizeof(un.sun_path)) {
        errno = ENAMETOOLONG;
        return(-1);
    }

    /* create a UNIX domain stream socket */
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        return(-1);

    /* fill socket address structure with our address */
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path, "%s%05ld", "/tmp/", (long)getpid());
    //printf("file is %s\n", un.sun_path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

    unlink(un.sun_path);        /* in case it already exists */
    if (bind(fd, (struct sockaddr *)&un, len) < 0) {
        rval = -2;
        goto errout;
    }
    if (chmod(un.sun_path, S_IRWXU) < 0) {
        rval = -3;
        do_unlink = 1;
        goto errout;
    }

    /* fill socket address structure with server's address */
    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path, name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
    if (connect(fd, (struct sockaddr *)&sun, len) < 0) {
        rval = -4;
        do_unlink = 1;
        goto errout;
    }
    return(fd);

errout:
    err = errno;
    close(fd);
    if (do_unlink)
        unlink(un.sun_path);
    errno = err;
    return(rval);
}


#define LISTENQ  64  /* Second argument to listen() */

/* open_listenfd - Open and return a listening socket on port. This
 *     function is reentrant and protocol-independent.
 *
 *     On error, returns -1 and sets errno.
 */
/* $begin open_listenfd */
int open_listenfd(char *port)
{
    struct addrinfo hints, *listp, *p;
    int listenfd, rc, optval=1;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* Accept TCP connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV;            /* ... using a numeric port arg. */
    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;  /* Socket failed, try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        if (close(listenfd) < 0) { /* Bind failed, try the next */
            fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
            return -1;
        }
    }


    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0) {
        return -1;
    }
    return listenfd;
}
/* $end open_listenfd */


/********************************
 * Client/server helper functions
 ********************************/
/*
 * open_clientfd - Open connection to server at <hostname, port> and
 *     return a socket descriptor ready for reading and writing. This
 *     function is reentrant and protocol-independent.
 *
 *     On error, returns -1 and sets errno.
 */
/* $begin open_clientfd */
int open_clientfd(char *hostname, char *port) {
    int clientfd, rc;
    struct addrinfo hints, *listp, *p;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;  /* Open a connection */
    hints.ai_flags = AI_NUMERICSERV;  /* ... using a numeric port arg. */
    hints.ai_flags |= AI_ADDRCONFIG;  /* Recommended for connections */
    if ((rc = getaddrinfo(hostname, port, &hints, &listp)) != 0) {
            fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", hostname, port, gai_strerror(rc));
            return -2;
    }

    /* Walk the list for one that we can successfully connect to */
    for (p = listp; p; p = p->ai_next) {

        /* Create a socket descriptor */
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue; /* Socket failed, try the next */

        /* Connect to the server */
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
            break; /* Success */
        if (close(clientfd) < 0) { /* Connect failed, try another */
            fprintf(stderr, "open_clientfd: close failed: %s\n", strerror(errno));
            return -1;
        }
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* All connects failed */
        return -1;
    else    /* The last connect succeeded */
        return clientfd;
}
/* $end open_clientfd */

void
doit(int readfd, int writefd)
{
	char	c;

	Write(writefd, &c, 1);
	if (Read(readfd, &c, 1) != 1)
		err_quit("read error");
}


int
main(int argc, char **argv)
{
	int		i, nloop, pipe1[2], pipe2[2];
	char	c;
	pid_t	childpid;

	if (argc != 2)
		err_quit("usage: lat_pipe <#loops>");
	nloop = atoi(argv[1]);

	Pipe(pipe1);
	Pipe(pipe2);

	if ( (childpid = Fork()) == 0) {
		for ( ; ; ) {		/* child */
			if (Read(pipe1[0], &c, 1) != 1)
				err_quit("read error");
		    Write(pipe2[1], &c, 1);
		}
		exit(0);
	}
		/* 4parent */
	doit(pipe2[0], pipe1[1]);

	Start_time();
	for (i = 0; i < nloop; i++)
		doit(pipe2[0], pipe1[1]);
	printf("latency: %.3f usec\n", Stop_time() / nloop);

	Kill(childpid, SIGTERM);
	exit(0);
}
/* end lat_pipe1 */

