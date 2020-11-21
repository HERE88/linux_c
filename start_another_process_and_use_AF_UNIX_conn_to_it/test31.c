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

int main()
{
    int listenfd, serv_fd;
    int ret_val;
    char recv_buf[2];


    printf("starting test30...\n");

    /* First start the client */
    system("./test30 &");

    //sleep(3); //just for test
    if((listenfd = serv_listen("/tmp/un_so_lmc")) < 0)
    {
        printf("serv_listen failed:%d - %s", errno, strerror(errno));
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


