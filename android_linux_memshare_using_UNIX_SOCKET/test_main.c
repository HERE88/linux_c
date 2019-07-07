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

int main()
{
    int ret_val = 0;
    //pid_t pid;

    int fd, un_listen_fd, un_fd;
	uid_t uid;
    char *data_ptr = NULL;

    region_size = 32;

    //创建一个共享内存（调用运行时库cutils中的接口！！）
    fd = ashmem_create_region(SHARED_REGION,region_size);
    if(fd < 0)
    {
        printf("ashmem_create_region error\n");
    }

	memset(&uid, 0, sizeof(uid_t));
    un_listen_fd = serv_listen(UN_SOCKET_NAME);
	if(un_listen_fd < 0)
	{
		perror("serv_listen error");
	}

	un_fd = serv_accept(un_listen_fd, &uid);
		if(un_fd < 0)
	{
		perror("serv_accept error");
	}

    //通过UNIX域套接字把匿名共享内存的fd传递给其他进程！！
	if(0 != send_fd(un_fd, fd))
	{
		perror("send_fd error");
	}

    //映射匿名共享内存
    data_ptr = mmap(NULL, region_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(data_ptr  != MAP_FAILED)
    {
        printf("mmap sharemem success\n");
        memset(data_ptr, 0x7f, region_size);
		close(fd); //映射完成后可以关闭fd

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

