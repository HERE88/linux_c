#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9090
#define MAXDATASIZE 1024

int main(void)
{
    int sockfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
    int num;
	time_t ticks;
    char buf[MAXDATASIZE];
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Creating socket failed.\n");
        exit(1);
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(9090);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Bind() error.\n");
        exit(1);
    }

    len = sizeof(client);
    // while(1)
    // {
        // num = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&client, &len);
        // if(num < 0)
        // {
            // perror("recvfrom() error.\n");
            // exit(1);
        // }
        // buf[num] = '\0';
        // printf("You got a message <%s> from client. \nIt's ip is %s, port is %d. \n", buf, inet_ntoa(client.sin_addr),htons(client.sin_port));
        // sendto(sockfd, "Welcome\n", 8, 0, (struct sockaddr *)&client, len);
        // if ( !strcmp(buf, "bye") ){
            // break;
        // }
    // }
	bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(9080);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

	while(1)
    {
		// ticks = time(NULL);
		// snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
		// sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&client, sizeof(client));
		// printf("send:%s\n", buf);
		// sleep(2);

		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
		sendto(sockfd, buf ,strlen(buf), 0, (struct sockaddr *)&client, len);
		printf("send:%s",buf);
		sleep(2);
    }
    close(sockfd);
}