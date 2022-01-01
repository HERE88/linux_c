//#include "LTE_V2X.h"

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


#include <netinet/in.h>
#include <arpa/inet.h>


static int sockfd;
static pthread_t recv_tid;

typedef void (*CB_RECV)(char *, int *);

typedef struct _THD_PARAM
{
    int fd;
    CB_RECV cb;
}THD_PARAM;


static void * receive_thd_proc(void * param)
{
    int  ret_val;
    unsigned char read_buf[65536];
    int  read_len;
    THD_PARAM temp;

    THD_PARAM * tp_tmp = (THD_PARAM *)param;
    temp.fd = tp_tmp->fd;
    temp.cb = tp_tmp->cb;
    free(tp_tmp);

    if(temp.fd <= 0) {
        printf("fd(%d) is not valid ", temp.fd);
        return NULL;
    }

    while(temp.fd)
    {
        read_len = read(temp.fd, read_buf, sizeof(read_buf));
        if (read_len > 0) {
            (*(temp.cb))(read_buf, &read_len);
            continue;
        } else {
            printf("read error:%d, %s \n", errno, strerror(errno));
            continue;
        }
    }

    return NULL;
}

int LTE_V2X_ParamInit()
{
    struct sockaddr_in client;
    struct sockaddr_in server;

    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_port = htons(10100);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket error.\n");
        return -1;
    }
    if(bind(sockfd, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        printf("bind error. \n");
        close(sockfd);
        return -1;;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(10200);

    if(connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("connect error.\n");
        close(sockfd);
        return -1;
    }

    return 0;  

}

int LTE_V2X_Tx_SendData(char* p_pSendData, int nDataLen)
{
    int  write_len = 0;
    int  try_counts = 0;

    unsigned char *bp;
    unsigned int  len_res;

     if( sockfd <= 0) {
        printf("fd(%d) is not valid.\n", sockfd);
        return -1;
    }

    if( !p_pSendData) {
        printf("p_pSendData is null");
        return -1;
    }

    if( nDataLen <= 0) {
        printf("nDataLen (%d) is not valid.\n", (int)nDataLen);
        return -1;
    }

    bp = (unsigned char *)p_pSendData;
    len_res = (unsigned int)nDataLen;

    while(try_counts < 100)
    {
        if (len_res == 0)
            break;

        write_len = write(sockfd, bp, len_res);
        if( write_len < 0 ) {
            printf("write to device failure error:%d-%s. fd:%d.\n", errno, strerror(errno), sockfd);
            usleep(10*1000);
            try_counts++;
            continue;
        }

        if( (unsigned int)write_len != len_res )  {
            bp += write_len;
            len_res -= write_len;
            usleep(10*1000);
            try_counts++;
            continue;
        } else {
            break;
        }
    }

    if (try_counts >= 100) {
        printf("write to device failure (timeout) len_res:%d, fd: [%d].\n", len_res, sockfd);
        return -1;
    }

    return 0;
}

int LTE_V2X_Rx_SetDevParam(CB_RECV func)
{
    int ret_val;
    
    pthread_attr_t thread_attr;

    THD_PARAM *tp = (THD_PARAM *)malloc(sizeof(THD_PARAM));
    tp->fd = sockfd;
    tp->cb = func;   
    
    ret_val = pthread_attr_init(&thread_attr);
    if (0 != ret_val) {
        printf("pthread_attr_init failed.\n");
        return -1;
    }

    ret_val = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (0 != ret_val) {
        printf("pthread_attr_setdetachstate failed");
        return -1;
    }

    ret_val = pthread_create(&recv_tid, &thread_attr, receive_thd_proc, tp);
    if (0 != ret_val) {
        printf("Thread creation failed");
        return -1;
    }

    pthread_attr_destroy(&thread_attr);
    return 0;
}


