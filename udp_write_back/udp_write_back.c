#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <errno.h>

int udp_connect(const char *host, const char *port, const char *remote_host, const char *remote_port)
{
    int sockfd, tc;
    struct sockaddr_in client;
    struct sockaddr_in server;

    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
	client.sin_addr.s_addr = inet_addr(host);
    client.sin_port = htons(atoi(port));
    
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() error");
        return -1;
    }
	if(bind(sockfd, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        printf("bind() error.");
        return -1;
    }
	
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(remote_host);
    server.sin_port = htons(atoi(remote_port));

    for (tc = 0; tc < 20; ++tc)
    {
        if(connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
            usleep(1000*1000);
            continue;
        } else {
            break;
        }
    }
    
    if(tc >= 20)
    {
        printf("connect() error");
        return -1;
    }

    return sockfd;
}

int main(int argc, char *argv[])
{
    int sockfd, ret;
    char buf[65536];

	int cnt = 0;
	
	sockfd = udp_connect("127.0.0.1","10200","127.0.0.1","10100");

    while(1)
    {

		if((ret = read(sockfd, buf, 65536)) == -1)
        {
            printf("read() error.\n");
            //exit(-1);
            continue;
        }

        if(write(sockfd, buf, ret) == ret)
            printf("write len:%d cnt:%d\n", ret, ++cnt);
        else
        {
            printf("write error:%d, %s\n", errno, strerror(errno));
            exit(-1);
        }


    }
    close(sockfd);

    return 0;
}