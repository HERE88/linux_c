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



int open_listenfd(char *port)
{
    struct addrinfo hints, *listp, *p;
    int listenfd, rc, optval=1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    hints.ai_flags |= AI_NUMERICSERV;
    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
        return -2;
    }

    for (p = listp; p; p = p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;

        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int));

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
        if (close(listenfd) < 0) {
            fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
            return -1;
        }
    }

    freeaddrinfo(listp);
    if (!p)
        return -1;

    if (listen(listenfd, 1024) < 0) {
        return -1;
    }
    return listenfd;
}


int main()
{
    int listenfd, serv_fd;
    int ret_val;
    char recv_buf[2];


    printf("starting pc5-tool...\n");

    /* First start the client */
    system("/home/wanghaitao/tmp/linux_C/test30 &");

    sleep(3); //just for test
    if((listenfd = open_listenfd("9200")) < 0)
    {
        printf("open_listenfd failed:%d - %s", errno, strerror(errno));
        return -1;
    }

    if((serv_fd = accept(listenfd, NULL, NULL)) < 0)
    {

        printf("accept failed:%d - %s", errno, strerror(errno));
        close(listenfd);

        return -1;
    }

    if((ret_val = read(serv_fd, recv_buf, 2)) != 2)
    {

        printf("read return:%d", ret_val);
        close(listenfd);
        close(serv_fd);

        return -1;
    }

    if(recv_buf[0] == 0x5F && recv_buf[1] == 0x00)
    {

        printf("received msg PASS from pc5-tool.\n");
        close(listenfd);
        close(serv_fd);

        return 0;
    }

    printf("received msg FAILED from pc5-tool.\n");
    close(listenfd);
    close(serv_fd);

    return -1;

}


