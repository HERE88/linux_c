#ifndef RIO_SOCKET_H_
#define RIO_SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define MAXBUF   2048  /* Max I/O buffer size */
#define LISTENQ  64  /* Second argument to listen() */

/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

/* Persistent state for the robust I/O (Rio) package */
#define RIO_BUFSIZE 2048
typedef struct {
    int rio_fd;                /* Descriptor for this internal buf */
    int rio_cnt;               /* Unread bytes in internal buf */
    char *rio_bufptr;          /* Next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE]; /* Internal buffer */
} rio_t;

int open_listenfd(char *port);
int open_clientfd(char *hostname, char *port);
void rio_readinitb(rio_t *rp, int fd) ;
ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

#endif


