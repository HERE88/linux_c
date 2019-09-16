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
#include <time.h>
#include <sys/time.h>



int main()
{
    //创建一个套结字
    int lfd,ret;
    lfd = socket(AF_INET,SOCK_DGRAM,0);
    int i,j;
    char buf[514] = {0};
    struct timeval startTime,endTime;
    float Timeuse;

    //封装套结字地址结构
    struct sockaddr_in saddr,caddr;
    saddr.sin_family = AF_INET;
    /*接收端端口号*/
    saddr.sin_port = htons(4444);
    /*接收端IP，此处设置为回环地址*/
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //绑定套结字地址到套结字描述字
    ret = bind(lfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(ret < 0)
    {
        perror("bind fail:");
        return -1;
    }
    //char buf[1024];
    int addrlen;

    for (j = 0; j < 10; ++j)
    {
        gettimeofday(&startTime,NULL);

        for(i=0;i<5000;i++)
        {
            //bzero(buf,sizeof(buf));
            addrlen = sizeof(caddr);
            /*接收发送端发送的数据*/
            ret = recvfrom(lfd,buf,514,0,(struct sockaddr*)&caddr,&addrlen);
        }

        gettimeofday(&endTime,NULL);
        Timeuse = 1000000*(endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
        Timeuse /= 1000000;
        printf("recv Timeuse = %f\n",Timeuse);
    }


    close(lfd);

    return 0;
}

