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

#include <sys/mman.h>

/* 2019-03-28, by wanghaitao */
#define SHARED_REGION "shared_region_for_gps"
#define UN_SOCKET_NAME "/tmp/un_socket_test"

size_t region_size = 32;

int main(int argc, char *argv[])
{
    int ret_val = 0;
	int fd, un_fd;

    char *data_ptr;

	un_fd = cli_conn(UN_SOCKET_NAME);
	if(un_fd < 0)
		perror("cli_conn error");

    //通过UNIX域套接字接收其他进程传递过来的fd
	fd = recv_fd(un_fd, NULL);
	if(fd < 0)
		perror("recv_fd error");

	printf("received fd:%d\n", fd);

    //映射匿名共享内存
    data_ptr = mmap(NULL, region_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(data_ptr  != MAP_FAILED)
    {
        printf("mmap sharemem success,value:%x\n", *data_ptr);
        close(fd); //一旦映射成功，就可以关闭fd

    }
    else
    {
        printf("mmap sharemem failed....'%s'\n", strerror(errno));
        close(fd);
    }

    while(1)
    {
        usleep(1000*1000);
    }

    close(un_fd); //关闭已连接描述符
    return ret_val;

}


