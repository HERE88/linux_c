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

#include <math.h>

// #define CONST_FILTER_FILE_NAME "test11.txt"
#define CONST_FILTER_FILE_NAME "test12.txt"

typedef unsigned char                       UINT8;
typedef unsigned short                      UINT16;
typedef unsigned  int                       UINT32;
#if 0
typedef struct _P_TT_DIAG_FILTER_CNF_para
{
    UINT32 filter_switch;/*{ < 0X00000000:="A7 & 4210 & 1643 Trace Close",
                               0X00000001:="A7 Trace Open",
                               0X00000002:="1643 Trace Open",
                               0X00000003:="A7 & 1643 Trace Open",
                               0X00000004:="4210 Trace Open",
                               0X00000005:="A7 & 4210 Trace Open",
                               0X00000006:="4210 & 1643 Trace Open",
                               0X00000007:="A7 & 1643 & 4210 Trace Open"
                               > }*/
    UINT32 sap_num;
    UINT32 _sap_id[21];//UINT32 _sap_id[sap_num]
    UINT8  print_level; /*{ < 0x00:="No output",0x01:="Error",0x02:="Warning",0x03:="Info">}*/
    UINT8  error_report_level;  /*{ < 0x00:="No output",0x01:="Error",0x02:="Warning",0x03:="Info">}*/
    UINT8  _filler[2];
}P_TT_DIAG_FILTER_CNF_para;
#endif

unsigned char hex_to_u8(unsigned char * p)
{
    unsigned char ret = 0;
    unsigned char n = 0;

    int i;
    for(i = 0; i< 2; ++i)
    {
        if(p[i] >= 'A' && p[i] <= 'F')
            ret = p[i] - 'A' + 10;
        else if(p[i] >= 'a' && p[i] <= 'f')
            ret = p[i] - 'a' + 10;
        else
            ret = p[i] - '0';

        n = n*16 + ret;
    }
    return n;
}

int main()
{
    int ret=0, index;
    FILE *fp = NULL;
    unsigned char buf[4096];
    unsigned char tmp[3] = {0};
    int n = 0;
    int i = 0;
    UINT32 filter_switch = 0;
    UINT32 sap_num = 0;
    UINT32 *_sap_id_p = NULL;
    UINT8 print_level = 0;
    UINT8 error_report_level = 0;

    unsigned char * ptr = buf;

    fp = fopen(CONST_FILTER_FILE_NAME, "r");
    if(fp == NULL)
    {
        printf("fopen file %s error\n", CONST_FILTER_FILE_NAME);
        return -1;
    }
    index = 0;
    while(!feof(fp))
    {
        n = fread(tmp, 1, 3, fp);
        // printf("n:%d\n", n);
        if((n == 3 && tmp[2] == ' ') || (n == 2 && tmp[1] != ' '))  //end with [FF] or [FF_] or [FF__] are well! (It's best to be the first situation.)
        {
            buf[index] = hex_to_u8(tmp);
            ++index;
        }
    }

    filter_switch = *(UINT32 *)ptr;
    printf("filter_switch:%d\n", filter_switch);

    sap_num = *(UINT32 *)(ptr + 4);
    printf("sap_num:%d\n", sap_num);

    _sap_id_p = (UINT32 *)(ptr + 4 + 4);
    for(i = 0; i < sap_num; ++i)
    {
        printf("sap[%d]:%d\n", i, _sap_id_p[i]);
    }

    print_level = *(UINT8 *)(ptr + 4 + 4 + sap_num*4);
    printf("print_level:%d\n", print_level);

    error_report_level = *(UINT8 *)(ptr + 4 + 4 + sap_num*4 + 1);
    printf("error_report_level:%d\n", error_report_level);

    if(index != (4 + 4 + sap_num*4 + 1 + 1 + 2))
    {
        printf("read filter file total items error!\n");
        return -1;
    }
    printf("index(param len):%d\n", index);
    return 0;

}