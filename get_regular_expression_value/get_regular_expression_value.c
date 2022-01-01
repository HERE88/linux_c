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

#define  OUTPUT_FILE_BAK_NAME  "/home/wanghaitao/tmp/linux_C/test_19700105161942_815070.bin_bak"
#define  OUTPUT_PATH           "/home/wanghaitao/tmp/linux_C"

void get_reg_exp_value(char * out, int size, int index)
{
    FILE * fp_mac;
    char line[256];

    char msg_buf[256];

    memset(line, 0x0, sizeof(line));

    memset(out,0,size);

    int maxRotationCountDigits = 3;
        //(elog_cfg_st.max_rotated_logs > 0) ? (int) (floor(log10(elog_cfg_st.max_rotated_logs) + 1)) : 0;

    if(index)
        snprintf(msg_buf, sizeof(msg_buf), "ls %s|sed \"s:^:%s/:\"|grep '^%s_.............._%.*d\\.tgz$'",
        OUTPUT_PATH, OUTPUT_PATH, OUTPUT_FILE_BAK_NAME, maxRotationCountDigits, index);
    else
        snprintf(msg_buf, sizeof(msg_buf), "ls %s|sed \"s:^:%s/:\"|grep '^%s_..............\\.tgz$'",
        OUTPUT_PATH, OUTPUT_PATH, OUTPUT_FILE_BAK_NAME);

    printf("msg_buf:[%s],len:[%d]\n", msg_buf, (int)strlen(msg_buf));

    fp_mac = popen(msg_buf, "r");
    while(fgets(line, sizeof(line), fp_mac) != NULL)
    {
        if(line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = 0;
        }
        printf("line:[%s],len:[%d]\n", line, (int)strlen(line));
        memcpy(out, line+strlen(OUTPUT_FILE_BAK_NAME)+1, 14);
    }

    pclose(fp_mac);

}


int main(int argc, char *argv[])
{
    int ret_val = 0;

    char buf[1024]={0};

    get_reg_exp_value(buf,1024,15);  // test_19700105161942_815070.bin_bak_20211103112233_015.tgz
    printf("buf:[%s]\n", buf);

    get_reg_exp_value(buf,1024,0);  // test_19700105161942_815070.bin_bak_20211103445566.tgz
    printf("buf:[%s]\n", buf);

    //usleep(0xfffffff);

    return 0;

}
