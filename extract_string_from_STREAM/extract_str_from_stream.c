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


#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

//////////////////////////////////////////////////////////////////////////////////////////////////
static int read_device(int fd, char *buf, int max_len, int timeout_sec);
static int is_str_complete(char *buf, int size);
static int extract_str_from_buf(char *buf, int buf_size, char *str_out);
static int read_one_complete_str(char *str_store);

//socket
int open_clientfd(char *hostname, char *port);

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


#define READ_BUFSIZE 1024
typedef struct {
    int read_fd;                /* Descriptor for this internal buf */
    //int read_cnt;               /* Unread bytes in internal buf */
    char *read_bufptr;          /* Next unread byte in internal buf */ /* means buffer IN not OUT */
    char read_buf[READ_BUFSIZE]; /* Internal buffer */
} read_t;

void read_init(read_t *rp, int fd)
{
    rp->read_fd = fd;
    //rp->read_cnt = 0;
    rp->read_bufptr = rp->read_buf;
}

read_t g_read_buf;
char  *g_str_begin, *g_str_end;

static int read_device(int fd, char *buf, int max_len, int timeout_sec)
{
    int ret;
    fd_set read_fds;
    struct timeval  tv_read = {0};
    struct timeval *timeout;

    if (fd <= 0 || buf == NULL)
        return -1;

    if (timeout_sec > 0) {
        tv_read.tv_sec = timeout_sec;
        tv_read.tv_usec = 0;
        timeout = &tv_read;
    } else
        timeout = NULL;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    printf("select ...\n");
    ret = select((fd + 1), &read_fds, NULL, NULL, timeout);
    if (ret < 0) {
        printf("select failed (%s)\n", strerror(errno));
        return -1;
    } else if (ret == 0) {
        printf("select timeout\n");
        return 0;
    } else if (FD_ISSET(fd, &read_fds)) {
        memset( buf, 0x00, max_len);
        ret = read(fd, buf, max_len - 1);
        if( ret <= 0 )
        {
            printf("read failed (%s)\n", strerror(errno));  //it is not real failed when recv EOF
            return -1;
        }
        //printf("read %d bytes: %s\n", ret, buf);
        return ret;
    } else {
        printf("select OK, but fd is not ready\n");
        return -1;
    }
}

static int is_str_complete(char *buf, int size)
{
    char *prefix = NULL;
    char *suffix = NULL;
    int i;

    if (size <= 4)
        return 0;
    for (i = 0; i < size - 1; i++) {
        if (buf[i] == '\r' && buf[i+1] == '\n') {
            if (prefix != NULL) {
                suffix = &buf[i];
            } else {
                prefix = &buf[i];
            }

            //success
            if (prefix != NULL && suffix != NULL) {
                g_str_begin = prefix;   //g_str_begin points to the head '\r'

                ///////g_str_end = suffix - 1; //g_str_end points to the letter BEFORE second '\r'
                g_str_end = suffix + 1; //g_str_end points to the second '\n'

                return 1;
            }
        }
    }

    return 0;
}

static int extract_str_from_buf(char *buf, int buf_size, char *str_out)
{
    int str_size;
    int remaining;
    char *ptr;

    if (!is_str_complete(buf, buf_size))
        return 0; //not complete, return ==0.

	//there is one complete str:
    str_size = g_str_end - g_str_begin + 1; //g_str_begin and g_str_end were set in function is_at_complete()
    memcpy(str_out, g_str_begin, str_size);
    str_out[str_size] = '\0';

    // skip ending \r\n
    ///////ptr = g_str_end + 3;
    ptr = g_str_end + 1;

    // partial AT string remaining ?
    remaining = g_read_buf.read_bufptr  - ptr;
    if (remaining > 0) {
        memmove(g_read_buf.read_buf, ptr, remaining);
    }
    g_read_buf.read_bufptr = g_read_buf.read_buf + remaining;

    return str_size;
}

static int read_one_complete_str(char *str_store)
{
    int ret_val = 0;

    //initialize the global variable
    g_str_begin = NULL;
    g_str_end = NULL;

    int at_str_size = 0;
    int max_read;

    if (g_read_buf.read_bufptr != g_read_buf.read_buf) {
        at_str_size = extract_str_from_buf(g_read_buf.read_buf,
            (g_read_buf.read_bufptr - g_read_buf.read_buf), str_store);
        if (at_str_size > 0)
            return at_str_size;
        // else continue to read
    }

    do {
        max_read = READ_BUFSIZE - (g_read_buf.read_bufptr - g_read_buf.read_buf);
        ret_val = read_device(g_read_buf.read_fd, g_read_buf.read_bufptr, max_read, 0);
        if (ret_val > 0) {
            g_read_buf.read_bufptr += ret_val;
            at_str_size = extract_str_from_buf(g_read_buf.read_buf,
                g_read_buf.read_bufptr - g_read_buf.read_buf, str_store);
            if (at_str_size == 0)
                continue;
            break;
        } else if (ret_val == 0) {
            printf("read_device timeout \n");
        } else if (ret_val < 0)
            printf("read_device ret_val < 0 \n");
            break;
    } while (1);

    return ret_val > 0 ? at_str_size : ret_val;  //if < 0, return error, > 0 return success, BUT never return 0.
}


int main(int argc, char *argv[])
{
    int fd, str_size;
    char str[READ_BUFSIZE];

    if((fd = open_clientfd("127.0.0.1", "7770")) < 0)
    {
        perror("open_clientfd error");
        exit(1);
    }

	//initialize
	memset(&g_read_buf, 0, sizeof(read_t));

	//open_fd
	read_init(&g_read_buf, fd);

    //read loop
    while(1)
    {
        if((str_size = read_one_complete_str(str)) < 0)
            break;

        str[str_size]='\0';
        printf("recv len:%d recv str:%s\n", str_size, str);

    }

    printf("main exit\n");

    return 0;
}
