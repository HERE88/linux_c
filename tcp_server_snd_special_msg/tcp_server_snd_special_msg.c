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

typedef void                                    VOID;
typedef char                                    CHAR;
typedef signed char                             BOOL;

typedef signed char                             SINT8;
typedef signed short                            SINT16;
typedef signed  int                             SINT32;
typedef signed long long                        SINT64;

typedef unsigned char                           UINT8;
typedef unsigned short                          UINT16;
typedef unsigned  int                           UINT32;
typedef unsigned long long                      UINT64;

#pragma pack (1)

/* LTE HLS MSG HEADER for standard interface */
typedef  struct _ELOG_HLS_LTE_MSG_HEAD_
{
    /* Tdiag_ext_f364 (DIAG: diag_type.h) */
    UINT16  segment_head;  //0xF3 64

    /* Tdiag_ext_msg_ver (DIAG: diag_type.h) */
    UINT16  segment_length; //length of this primitive, include param+peer, but not include the 0xF364(2 bytes).
    UINT8   interface_flag;
    UINT8   interface_version;
    UINT16  message_sn;  //cplogctrl <-> tt

    /* Tdiag_ext_msg_head (DIAG: diag_type.h) */
    UINT16  temp_total_msg_len;  //length form here byte, include param+peer.
    UINT16  ucDiagId;
    UINT16  ucWaveId;

    UINT8   src_task_id;
    UINT8   src_process_id;
    UINT8   dest_task_id;
    UINT8   dest_process_id;
    UINT8   ucUeId;
    UINT8   ucResv;
    UINT32  sap_id;
    UINT32  prim_id;
    UINT32  timestamp;

    /* part of msg head which are newly added: */
    UINT32  ulFn;
    UINT16  usOffset;

    UINT16  param_len;
    UINT16  usPeerLen;
    UINT16  usfreeHeaderlen;
} ELOG_HLS_MSG_LTE_HEAD;
#pragma pack ()

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
    if (listen(listenfd, 64) < 0) {
        return -1;
    }
    return listenfd;
}
/* $end open_listenfd */



int main()
{
    ELOG_HLS_MSG_LTE_HEAD msg_snd;

    int listen_fd, connect_fd;
    int temp_fd;


    memset(&msg_snd, 0, sizeof(msg_snd));

    msg_snd.segment_head = 0x64F3;
    msg_snd.segment_length = htons(sizeof(msg_snd) - sizeof(UINT16));
    msg_snd.prim_id = htonl(12100);
    msg_snd.interface_version = 0x02;

    // listen_fd = open_listenfd("9998");
    // connect_fd = accept(listen_fd, 0, 0);
    // write(connect_fd, &msg_snd, sizeof(msg_snd));

    temp_fd = open("wang.txt", O_WRONLY | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR);
    write(temp_fd, &msg_snd, sizeof(msg_snd));

    sleep(10);


    return 0;


}