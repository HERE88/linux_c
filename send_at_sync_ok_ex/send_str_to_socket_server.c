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
    char *send = "\r\nWANGHAITAO\r\n";
    char recv_buf[256] = {0};

    int listen_fd, connect_fd;


    listen_fd = open_listenfd("7770");

    connect_fd = accept(listen_fd, 0, 0);


    memset(recv_buf, 0, sizeof(recv_buf));
    read(connect_fd, recv_buf, sizeof(recv_buf));
    printf("recv_buf:%s\n", recv_buf);



    write(connect_fd, send, strlen(send));

    sleep(3);

    send = "\r\nMORNINGCORE\r\n";
    write(connect_fd, send, strlen(send));

    sleep(3);

    send = "\r\nLIYING\r\n              \r\nHELLOWOLD\r\n";
    write(connect_fd, send, strlen(send));

    sleep(3);

    send = "\r\nTHANK";
    write(connect_fd, send, strlen(send));

    sleep(3);

    send = "YOU\r\n";
    write(connect_fd, send, strlen(send));

    sleep(3);
    send = "\r\nOK\r\n";
    write(connect_fd, send, strlen(send));


    //read AT
    memset(recv_buf, 0, sizeof(recv_buf));
    read(connect_fd, recv_buf, sizeof(recv_buf));
    printf("recv_buf2:%s\n", recv_buf);

    //read "Test for pre-cfg.\n"
    memset(recv_buf, 0, sizeof(recv_buf));
    read(connect_fd, recv_buf, sizeof(recv_buf));
    printf("recv_buf2:%s\n", recv_buf);

    sleep(3);
    send = "\r\nbuf2 will send\r\n";
    write(connect_fd, send, strlen(send));

    sleep(3);
    send = "\r\nOK\r\n";
    write(connect_fd, send, strlen(send));


    pause();


    return 0;


}
