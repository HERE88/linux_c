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


#if !defined(OPT_PLATFORM_WIN32)
    typedef void                                    VOID;
    typedef char                                    CHAR;
#if !defined(OPT_SFW_EXTERNAL_TYPE_DEF)
    typedef signed char                             BOOL;
#endif
#endif
    typedef signed char                             SINT8;
    typedef signed short                         SINT16;
    typedef signed  int                         SINT32;
#ifdef WIN32
    typedef double                                  SINT64;
#else
    typedef signed long long                        SINT64;
#endif
    typedef unsigned char                           UINT8;
    typedef unsigned short                       UINT16;
#if !defined(OPT_PLATFORM_WIN32)
    typedef unsigned  int                       UINT32;
#ifdef WIN32
    typedef double                                  UINT64;
#else
    typedef unsigned long long                      UINT64;
#endif
#endif

#define LOG_PRINT(...) printf(__VA_ARGS__)

double recv_rate = 0.0;


int   fd_sock_pc =-1;
int   recv_grp_cnt = 0;
char  msg_send_buf[2] = {0};
#define MSG_TO_PC_PREFIX  0x5F
#define MSG_TO_PC_PASS    0x00
#define MSG_TO_PC_FAILED  0xFF

//////////////////////////////////////////////////////////////////////
#define MAXBUF   2048  /* Max I/O buffer size */
#define LISTENQ  64  /* Second argument to listen() */

/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;


ssize_t rio_writen(int fd, void *usrbuf, size_t n) ;




 void test(void)
 {
     int ret_val;


    if(recv_rate >= 95.0)  /* PASS(=95.0%?) */
    {
        memset(msg_send_buf, 0, sizeof(msg_send_buf));
        msg_send_buf[0] = MSG_TO_PC_PREFIX;
        msg_send_buf[1] = MSG_TO_PC_PASS;

        /* write msg PASS to socket */
        if ((ret_val = rio_writen(fd_sock_pc, msg_send_buf, sizeof(msg_send_buf))) != sizeof(msg_send_buf))
        {
            LOG_PRINT("rio_writen(msg PASS to PC) error: %s\n", strerror(errno));
        }
        else
        {
            LOG_PRINT("write msg(PASS) to socket success! len:[%lu], will exit(2)\n", sizeof(msg_send_buf));
        }
        close(fd_sock_pc);
        usleep(200 * 1000); /* could use SO_LINGER instead. */

        exit(2);
    }
    else if(recv_grp_cnt >= 1)  /* FAILED */
    {
        memset(msg_send_buf, 0, sizeof(msg_send_buf));
        msg_send_buf[0] = MSG_TO_PC_PREFIX;
        msg_send_buf[1] = MSG_TO_PC_FAILED;

        /* write msg FAILED to socket */
        if ((ret_val = rio_writen(fd_sock_pc, msg_send_buf, sizeof(msg_send_buf))) != sizeof(msg_send_buf))
        {
            LOG_PRINT("rio_writen(msg FAILED to PC) error: %s\n", strerror(errno));
        }
        else
        {
            LOG_PRINT("write msg(FAILED) to socket success! len:[%lu], will exit(3)\n", sizeof(msg_send_buf));
        }
        close(fd_sock_pc);
        usleep(200 * 1000); /* could use SO_LINGER instead. */

        exit(3);
    }

    recv_grp_cnt++;



 }


ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = (char *)usrbuf;

    while (nleft > 0) {
    if ((nwritten = write(fd, bufp, nleft)) <= 0) {
        if (errno == EINTR)  /* interrupted by sig handler return */
        nwritten = 0;    /* and call write() again */
        else
        return -1;       /* errorno set by write() */
    }
    nleft -= nwritten;
    bufp += nwritten;
    }
    return n;
}



int main()
{

        if((fd_sock_pc = open("/tmp/fifo_lmc", O_WRONLY)) < 0)
        {

            printf("open failed:%d - %s", errno, strerror(errno));

            return -1;
        }
        LOG_PRINT("open O_WRONLY success, fd:%d\n", fd_sock_pc);



/////////////////////////////////////////////////////////////////////
sleep(1);
LOG_PRINT("1st sleep end\n");
recv_rate = 18.8;
test();

sleep(1);
LOG_PRINT("2st sleep end\n");

recv_rate = 98.8;
test();
sleep(1);
LOG_PRINT("3st sleep end\n");

recv_rate = 99.8;
test();

    return 0;

}