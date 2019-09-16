#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

int main()
{
    //创建一个套结字
    int lfd,ret;
    lfd = socket(AF_INET,SOCK_DGRAM,0);
    int i = 0;
    int j = 0;

    struct timeval startTime,endTime;
    float Timeuse;

    //封装套结字地址结构
    struct sockaddr_in saddr,caddr;
    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(5555);
    caddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //绑定套结字地址到套结字描述字
    ret = bind(lfd,(struct sockaddr*)&caddr,sizeof(caddr));
    if(ret < 0)
    {
        perror("bind fail:");
        return -1;
    }
    //封装套结字地址结构

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(4444);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    char buf[514]="test udp protcol !";
    int addrlen;

    for (j = 0; j < 10; ++j)
    {
        gettimeofday(&startTime,NULL);

        for(i=0;i<5000;i++)
        {
            sendto(lfd,buf,514,0,(struct sockaddr*)&saddr,sizeof(saddr));
        }


        gettimeofday(&endTime,NULL);
        Timeuse = 1000000*(endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
        Timeuse /= 1000000;
        printf("send Timeuse = %f\n",Timeuse);
    }


    while(1)
    {
        //printf("send %d th package!\n", ++i);
        /*一直向saddr的地址（接收端IP和端口号），发送udp包*/
        sendto(lfd,buf,514,0,(struct sockaddr*)&saddr,sizeof(saddr));
    }



    close(lfd);

    return 0;
}




