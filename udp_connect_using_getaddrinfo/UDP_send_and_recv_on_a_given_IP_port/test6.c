#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 9080

#define MAXDATASIZE 100

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
        exit(1);
    }
	if(bind(sockfd, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        printf("bind() error.");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(remote_host);
    server.sin_port = htons(atoi(remote_port));

    for (tc = 0; tc < 20; ++tc)
    {
        if(connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
        {
            usleep(1000*1000);
            continue;  //注意有些系统connect失败后的socket状态是不确定的，需要重新创建！
        }
        else
        {
            break;
        }
    }

    if(tc >= 20)
    {
        printf("connect() error");
        exit(1);
    }

    return sockfd;
}

int main(int argc, char *argv[])
{
    int sockfd, num;
    char buf[MAXDATASIZE];
    // struct sockaddr_in server;
	// struct sockaddr_in client;

	// bzero(&client, sizeof(client));
    // client.sin_family = AF_INET;
    // client.sin_port = htons(9080);
    // client.sin_addr.s_addr = inet_addr("127.0.0.1");

    // if((sockfd=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    // {
        // printf("socket() error\n");
        // exit(1);
    // }
	// if(bind(sockfd, (struct sockaddr *)&client, sizeof(client)) == -1)
    // {
        // perror("Bind() error.\n");
        // exit(1);
    // }


    // bzero(&server, sizeof(server));
    // server.sin_family = AF_INET;
    // server.sin_port = htons(9090);
    // server.sin_addr.s_addr = inet_addr("127.0.0.1");
    // if(connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    // {
        // printf("connect() error.\n");
        // exit(1);
    // }

	sockfd = udp_connect("127.0.0.1","9080","127.0.0.1","9090");

    while(1)
    {
        // if((num = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
        // {
            // printf("recv() error.\n");
            // exit(1);
        // }
		if((num = read(sockfd, buf, MAXDATASIZE)) == -1)
        {
            printf("read() error.\n");
            exit(1);
        }

        buf[num] = '\0';
        printf("Server Message(read):%s", buf);
    }
    close(sockfd);
}