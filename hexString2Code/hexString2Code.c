#define _GNU_SOURCE

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

#include <math.h>

#include <stdbool.h>

#include <sys/time.h>

#include <sys/resource.h>  // setrlimit, add 20210917

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

#define LOG_INFO_E(x) LOG_PRINT x
#define LOG_PRINT(...) PRINT_INFO(__VA_ARGS__)


void PRINT_INFO(char *format, ...)
{
    char new_fm[128];
    int len;
    va_list ap;

    len=strlen(format);
    va_start(ap,format);

    strncpy(new_fm,format,sizeof(new_fm)-1);
    if(new_fm[len-1] != '\n')
    {
        new_fm[len]='\n';
        new_fm[len+1]=0;
    }

    vprintf(new_fm,ap);
    va_end(ap);
}

/*********************************************INS BELOW*********************************************/

/* Convert HEX string to byte, e.g. "31" -> 0x31 */
static int hexString2Code(const unsigned char *src, char *dst, int len) {
    int i;
    int out_len = 0;

    if(!src || !dst) {
        return 0;
    }

    for(i=0; i<len/2; i++) {
        if((*src<='9') && (*(src+1)<='9')) {
            *dst++ = ((*src-'0') << 4) | (*(src+1)-'0');
        } else if((*src<='9') && (*(src+1)<='F')) {
            *dst++ = ((*src-'0') << 4) | (*(src+1)-'A'+10);
        } else if((*src<='F') && (*(src+1)<='9')) {
            *dst++ = ((*src-'A'+10) << 4) | (*(src+1)-'0');
        } else if((*src<='F') && (*(src+1)<='F')) {
            *dst++ = ((*src-'A'+10) << 4) | (*(src+1)-'A'+10);
        }
        src +=2;
        out_len++;
    }
    *dst = '\0';

    return out_len;
}



int main(int argc, char *argv[])
{
    int ret_val = 0;

    char src[]="414243";  // 0x41(A) 0x42(B) 0x43(C)
    char dst[1024] = {0};

    hexString2Code(src, dst, strlen(src));

    printf("%s\n", dst);


    //usleep(0xfffffff);
    return 0;
}
