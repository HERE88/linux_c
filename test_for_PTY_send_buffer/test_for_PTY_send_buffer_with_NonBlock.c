#define _XOPEN_SOURCE
#include <unistd.h>
#include <fcntl.h>

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


#if !defined(OPT_PLATFORM_WIN32)
    typedef void                                    VOID;
    typedef char                                    CHAR;
#if !defined(OPT_SFW_EXTERNAL_TYPE_DEF)
    typedef signed char                             BOOL;
#endif
#endif
    typedef signed char                             SINT8;
    typedef signed short                         SINT16;
    typedef signed  int                         SINT32;
#ifdef WIN32
    typedef double                                  SINT64;
#else
    typedef signed long long                        SINT64;
#endif
    typedef unsigned char                           UINT8;
    typedef unsigned short                       UINT16;
#if !defined(OPT_PLATFORM_WIN32)
    typedef unsigned  int                       UINT32;
#ifdef WIN32
    typedef double                                  UINT64;
#else
    typedef unsigned long long                      UINT64;
#endif
#endif


#define DEV_TTY_NAME "TTY_TEST_MUX"

static int set_ptm_attr( int fd )
{
    int     ret_val = 0;
    struct termios  options;


    if( fd < 0 )
    {
        return -1;
    }

    ret_val = tcgetattr(fd, &options);

    if( 0 == ret_val )
    {
        // set raw input
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
        /*  for test 3 begin */
        options.c_iflag = 0;
        /*  for test 3 end */

        // set raw output
        options.c_oflag &= ~OPOST;
        options.c_oflag &= ~OLCUC;
        options.c_oflag &= ~ONLRET;
        options.c_oflag &= ~ONOCR;
        options.c_oflag &= ~OCRNL;

        ret_val = tcsetattr(fd, TCSANOW, &options);
    }

    /* Change the permission of slave. */
    if( 0 == ret_val )
    {
        ret_val = grantpt(fd);
    }

    /* Unlock slave. */
    if( 0 == ret_val )
    {
        ret_val = unlockpt(fd);
    }

    return ret_val;
}

void set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
	int		val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
	{
		printf("fcntl F_GETFL error");
		return;
	}

	val |= flags;		/* turn on flags */

	if (fcntl(fd, F_SETFL, val) < 0)
	{
		printf("fcntl F_SETFL error");
		return;
	}
}


int main()
{
    SINT32 ret = 0;
    int i;
    //use PTY for test MUX frame![wang]
    int m_fd = 0;
    char *pts_name_ptr = NULL;

    m_fd = open("/dev/ptmx", O_RDWR);  // "/dev/ptmx"
    if(m_fd < 0)
    {
        perror("open ptmx error");
        return -1;
    }

    pts_name_ptr = ptsname(m_fd);
    if( NULL == pts_name_ptr )
    {
        printf("ptsname errno =%d, =%s\n", errno, strerror(errno));
        return -1;
    }

    unlink(DEV_TTY_NAME);
    ret = symlink(pts_name_ptr, DEV_TTY_NAME);
    if(ret != 0)
    {
        return -1;
    }
    else
    {
        ret = chmod(DEV_TTY_NAME, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if( ret != 0 )
        {
            printf("chmod errno =%d, %s\n", errno, strerror(errno));
        }
        // if(chown(DEV_TTY_NAME, 1000, 1001) < 0)
        // {
            // printf("chown errno =%d, %s\n", errno, strerror(errno));
        // }
        else
        {
            printf("success m_pts_name_a=%s\n" , DEV_TTY_NAME);
        }
    }

    ret = set_ptm_attr(m_fd);
    if(ret != 0)
    {
        perror("set_ptm_attr error");
        return -1;
    }
	
	set_fl(m_fd, O_NONBLOCK);

    char buf[1024];
    for(i = 0 ; i < 2000; ++i)
    {
        //memset
        snprintf(buf, sizeof(buf) -1 , "WANGHAITAO=%04d\n", i);
        ret = write(m_fd, buf, strlen(buf));
		if(ret <= 0 )
		{
			printf("write error, index:%d\n" , i);
		}
		else
			printf("write:WANGHAITAO=%04d\n" , i);
    }

    printf("start sleeping...\n");

    sleep(0xffffffff);
    return 0;

}


