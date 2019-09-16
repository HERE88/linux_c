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
#include <netdb.h>
#include  <sys/socket.h>

#define MAXSLEEP 128

int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen)
{
    int numsec;

    /*
     * Try to connect with exponential backoff.
     */
    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
        if (connect(sockfd, addr, alen) == 0) {
            /*
             * Connection accepted.
             */
            return(0);
        }

        /*
         * Delay before trying again.
         */
        if (numsec <= MAXSLEEP/2)
            sleep(numsec);
    }
    return(-1);
}

void
print_protocol(struct addrinfo *aip)
{
    printf("protocol: ");
    switch (aip->ai_protocol) {
    case 0:
        printf("default");
        break;
    case IPPROTO_TCP:
        printf("TCP\n");
        break;
    case IPPROTO_UDP:
        printf("UDP\n");
        break;
    case IPPROTO_RAW:
        printf("raw\n");
        break;
    default:
        printf("unknown (%d)\n", aip->ai_protocol);
    }
}


int udp_connect(const char *host, const char *serv)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", host, serv, gai_strerror(n));
        return -2;
    }
    ressave = res;

    /* Walk the list for one that we can successfully connect to */
    do{
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(sockfd < 0)
            continue; /* Socket failed, try the next */

        if(connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
        {
            print_protocol(res);
            break; /* Success */
        }


        if(close(sockfd) < 0)
        { /* Connect failed, try another */
            fprintf(stderr, "open_clientfd: close failed: %s\n", strerror(errno));
            return -1;
        }
    }while((res = res->ai_next) != NULL);

    if(res == NULL)
    {
        printf("udp_connect error for %s, %s\n", host, serv);
        return -1; /* All connects failed */
    }

    freeaddrinfo(ressave);

    return sockfd;
}


int main()
{
    int sockfd;
    ssize_t n;
    char buff[1024];
    socklen_t len;
    //struct sockaddr_storage cliaddr;

    if((sockfd = udp_connect("127.0.0.1", "9080")) < 0)
    {
        printf("udp_connect error\n");
        return -1;
    }
    else
    {
        printf("udp_connect success\n");
    }

    write(sockfd, "", 1);   /* send 1-byte datagram */
    for(;;)
    {
        //len = sizeof(cliaddr);
        n = recvfrom(sockfd, buff, 1024, 0, NULL, NULL);
        printf("recv:%s", buff);
    }

    return 0;

}