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
/*********************************************INS BELOW*********************************************/

#define LOG_INFO(x)  LOG_PRINT x

#define LOG_PRINT(...) printf(__VA_ARGS__)

void mem2hex(const unsigned char* source, char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;  // between 0~15
        lowByte  = source[i] & 0x0f; // between 0~15

        highByte += '0';  // 0-->'0'
        lowByte  += '0';

        if (highByte > '9')
            dest[i * 2] = highByte + 0x07;  // '9'(57) --> 'A'(65)
        else
            dest[i * 2] = highByte;

        if (lowByte > '9')
            dest[i * 2 + 1] = lowByte + 0x07;  // '9'(57) --> 'A'(65)
        else
            dest[i * 2 + 1] = lowByte;
    }
}

void mem2hex_b(const unsigned char* source, char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;  // between 0~15
        lowByte  = source[i] & 0x0f; // between 0~15

        highByte += '0';  // 0-->'0'
        lowByte  += '0';

        if (highByte > '9')
            dest[i * 3] = highByte + 0x07;  // '9'(57) --> 'A'(65)
        else
            dest[i * 3] = highByte;

        if (lowByte > '9')
            dest[i * 3 + 1] = lowByte + 0x07;  // '9'(57) --> 'A'(65)
        else
            dest[i * 3 + 1] = lowByte;

        dest[i * 3 + 2] = 0x20; // ' '(blank)
    }
}

int main()
{
    char tmp[64] = "ABCD";
 	
    char d[1024] = {0};

	mem2hex(tmp, d, 64);
    printf("[%s]\n", d);

    mem2hex_b(tmp, d, 64);
	printf("[%s]\n", d);
    return 0;

}


