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

int main()
{
    SINT32 ret = 0;
    int i;

    int fd = 0;

	char buf[10240];

    fd = open(DEV_TTY_NAME, O_RDWR);  // "/dev/ptmx"
    if(fd < 0)
    {
         perror("open  error");
        return -1;
    }

	memset(buf, 0, sizeof(buf));
	read(fd, buf, sizeof(buf));
	printf("read:%s", buf);
	

    printf("read sleeping...\n");

    sleep(0xffffffff);
    return 0;

}


