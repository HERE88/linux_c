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



//(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main()
{
    int serv_fd;
    int ret_val;
    char recv_buf[2];


    printf("starting test30...\n");

    unlink("/tmp/fifo_lmc");
    if((mkfifo("/tmp/fifo_lmc", 0777) < 0) && (errno != EEXIST))
    {
        printf("can't create /tmp/fifo_lmc\n");
        
        return -1;
    }

    /* First start the client */
    system("./test30 &");

    // printf("sleep(3) begin\n");
    // sleep(3);
    // printf("sleep(3) end\n");
    
    if((serv_fd = open("/tmp/fifo_lmc", O_RDONLY)) < 0)
    {

        printf("open failed:%d - %s", errno, strerror(errno));
        unlink("/tmp/fifo_lmc");

        return -1;
    }
    LOG_PRINT("open O_RDONLY success, fd:%d\n", serv_fd);
    
    // sleep(10);
    if((ret_val = read(serv_fd, recv_buf, 2)) != 2)
    {

        printf("read return:%d", ret_val);
        
        close(serv_fd);

        return -1;
    }

    if(recv_buf[0] == 0x5F && recv_buf[1] == 0x00)
    {

        printf("received msg PASS from pc5-tool.\n");
        
        close(serv_fd);

        return 0;
    }

    printf("received msg FAILED from pc5-tool.\n");
    
    close(serv_fd);

    return -1;

}


