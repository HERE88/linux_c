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


/*******************************************************************************************/
typedef signed char                             BOOL;
typedef signed char                          SINT8;
typedef signed short                         SINT16;
typedef signed  int                          SINT32;
typedef unsigned char                        UINT8;
typedef unsigned short                       UINT16;
typedef unsigned  int                        UINT32;
#ifndef TRUE
#define TRUE                                    1
#endif
#ifndef FALSE
#define FALSE                                   0
#endif

//char    optArr[] = {'y','d','s','t','n','p','r'};
UINT16  tmsArr[] = {20,50,100,200,300,400,500,600,700,800,900,1000};

#define PDCP_PEER_SDU_HEAD_LEN 48

//config file path
char    mConfigFileA[128] = {0};

//1=autorun, 0=manul
UINT8  autoRunState = 0;
UINT8  cfgAvailNum = 0;

#define DEFAULT_PROGRESS_NAME "wang"

typedef struct ConfigFileSt
{
    /* 1=available,0=invaliable */
    UINT16  itemNum;

    /* 1=serial,0=parallel */
    UINT16  parallel;

    //info (StartUpOpt)
    UINT32  sourceL2Id;
    UINT32  destinationL2Id;
    UINT16  sndPkgSize;
    UINT16  sndPkgNum;
    UINT16  sndIntervalTime;
    UINT8   sndPkgPri;
    SINT8   power;
} configFileSt;

//#define DEFAULT_CONFIG_FILE_PATH "/system/etc/pc5-setup.cfg"
#define DEFAULT_SETUP_FILE_PATH "pc5-setup.ini"

#define DEFAULT_CONFIG_MAX_NUM     10  //默认一次最多10个配置
configFileSt  cfgFileSt[DEFAULT_CONFIG_MAX_NUM];


/*******************************************************************************************/
static SINT32 cfgFileReadAndCheck(const char *configFilePtr);
static SINT32 cfgGetParamAndValue( char *linePtr, char **paramPtr, char **valuePtr );
static SINT32 cfgInitialize();
//static void delElementInArray(char elem, char * elem_arr, int a_len);
static BOOL isInSet(UINT16 tms, UINT16 * tms_arr, UINT8 a_len);


/*******************************************************************************************/
static BOOL isInSet(UINT16 tms, UINT16 * tms_arr, UINT8 a_len)
{
    int i = 0;
    BOOL ret_val = FALSE;
    for(i = 0;i < a_len; ++i)
    {
        if(tms == tms_arr[i])
        {
            ret_val = TRUE;
            break;
        }
    }

    return ret_val;
}

/*static void delElementInArray(char elem, char * elem_arr, int a_len)
{
    int i = 0;
    for(i = 0; i < a_len; ++i)
    {
        if(elem_arr[i] == elem)
        {
            elem_arr[i] = 0;
        }
    }
}*/

static SINT32 cfgGetParamAndValue( char *linePtr, char **paramPtr, char **valuePtr )
{
    char    *p1 = NULL;             // buffer pointer variable
    char    *p2 = NULL;             // buffer pointer variable
    SINT32   retVal = 0;           // error code

    // confirm parameter's validity
    if( !linePtr || !paramPtr || !valuePtr )
    {
        printf("cfgGetParamAndValue: param error. \n");
        return -1;
    }

    // remove white space
    while( ' ' == *linePtr )
        linePtr++;

    // confirm the line is comment or not
    if( '#' == *linePtr )
    {
        return -2;  //若有"#"默认为注释，该函数会返回错误-2
    }

    // confirm the line to include "=" or not
    p1 = strstr(linePtr, "="); //返回第一次出现"="的地方
    if( p1 )
    {
        *p1 = '\0';

        p2 = p1 + 1;
        p1--;

        // remove white space
        while( ' ' == *p1 )
        {
            *p1 = '\0';
            p1--;
        }
        *paramPtr = linePtr;

        while( ' ' == *p2 )
        {
            p2++;
        }
        while( strlen(p2) > 0 && (' ' == p2[strlen(p2) - 1] || '\r' == p2[strlen(p2) - 1] || '\n' == p2[strlen(p2) - 1]) )
        {
            p2[strlen(p2) - 1] = '\0';  //strlen(p2)每次都会改变
        }
        *valuePtr = p2;
    }
    else
    {
        retVal = -3;  //没有"="时也认为错误，返回错误-3
    }

    return retVal;
}

SINT32 cfgFileReadAndCheck(const char *configFilePtr)
{
    SINT32  retVal = 0;         // error code
    FILE    *fp = NULL;                 // file handle
    char    lineA[256];                 // line buffer variable
    char    *paramPtr = NULL;           // buffer pointer variable
    char    *valuePtr = NULL;           // buffer pointer variable
    long    retAtol;
    int     i = 0;

    char    parallel[32] = {0};
    char    y[32] = {0};
    char    d[32] = {0};
    char    s[32] = {0};
    char    n[32] = {0};
    char    t[32] = {0};
    char    r[32] = {0};
    char    p[32] = {0};

    // confirm parameter's validity
    if(!configFilePtr || 0 == strlen(configFilePtr))
    {
        printf("configFilePtr is NULL, will use default path.\n");
        memset(mConfigFileA, 0x00, sizeof(mConfigFileA));
        strncpy(mConfigFileA, DEFAULT_SETUP_FILE_PATH, strlen(DEFAULT_SETUP_FILE_PATH));
    }
    else
    {
        printf("configFilePtr is not NULL %s\r\n", configFilePtr);
        memset(mConfigFileA, 0x00, sizeof(mConfigFileA));
        strncpy(mConfigFileA, configFilePtr, strlen(configFilePtr));
    }

    printf("try to open config file %s.\r\n", mConfigFileA);

    /* Read config info from the config file. */
    if((fp = fopen(mConfigFileA, "r")) != NULL)
    {
        while(!feof(fp))
        {
            // clear line-buffer
            memset(lineA, 0x00, sizeof(lineA));
            // get one line from file
            fgets(lineA, sizeof(lineA) - 1, fp);
            // separate the line into parameter and value
            retVal = cfgGetParamAndValue(lineA, &paramPtr, &valuePtr);

            // confirm return value and parameter's validity
            if(0 == retVal && paramPtr && strlen(paramPtr) > 0)
            {
                //首先判断有无autoRunState，且是否等于1
                if(0 == strcmp(paramPtr, "autorun_state"))
                {
                    retAtol = atol(valuePtr);
                    if(retAtol != 1)
                    {
                        printf("autorun_state is not set to 1.\n");
                        fclose(fp);
                        return -1;  //立即退出
                    }
                    else
                    {
                        autoRunState = 1;  //autorun_state置为1，表示开启了自动发送
                        continue; //直接进入下一个while循环
                    }
                }

                //除autorun_state之外其他的字段判断处理
                for(i = 0; i < DEFAULT_CONFIG_MAX_NUM; ++i)
                {
                    snprintf(parallel,sizeof(parallel),"parallel%d",i);
                    snprintf(y,sizeof(y),"source_l2_id%d",i);
                    snprintf(d,sizeof(d),"destination_l2_id%d",i);
                    snprintf(s,sizeof(s),"snd_pkg_size%d",i);
                    snprintf(n,sizeof(n),"snd_pkg_num%d",i);
                    snprintf(t,sizeof(t),"snd_interval_time%d",i);
                    snprintf(r,sizeof(r),"snd_pkg_pri%d",i);
                    snprintf(p,sizeof(p),"power%d",i);

                    if(0 == strcmp(paramPtr, (char *)parallel))
                    {
                        retAtol = atol(valuePtr);
                        if(1 == retAtol)
                        {
                            cfgFileSt[i].parallel = retAtol;
                            printf("cfg read [parallel%d]=[%d]\n", i, cfgFileSt[i].parallel);
                        }
                        else
                        {
                            cfgFileSt[i].parallel = 0;
                            printf("cfg read [parallel%d]=[%d]\n", i, cfgFileSt[i].parallel);
                        }
                        cfgFileSt[i].itemNum += 1;
                        break;
                    }
                    else if(0 == strcmp(paramPtr, (char *)y))
                    {
                        if((cfgFileSt[i].sourceL2Id = inet_addr((char *)valuePtr)) != INADDR_NONE)
                        {
                            printf("cfg read [source_l2_id%d]=[%u]\n", i, cfgFileSt[i].sourceL2Id);
                            cfgFileSt[i].itemNum += 1;
                            break;
                        }
                        else
                        {
                            printf("[source_l2_id%d] invalid!\n", i);
                            fclose(fp);
                            return -1;
                        }
                    }
                    else if(0 == strcmp(paramPtr, (char *)d))
                    {
                        if((cfgFileSt[i].destinationL2Id = inet_addr((char *)valuePtr)) != INADDR_NONE)
                        {
                            printf("cfg read [destination_l2_id%d]=[%u]\n", i, cfgFileSt[i].destinationL2Id);
                            cfgFileSt[i].itemNum += 1;
                            break;
                        }
                        else
                        {
                            printf("[destination_l2_id%d] invalid!\n", i);
                            fclose(fp);
                            return -1;
                        }

                    }
                    else if(0 == strcmp(paramPtr, (char *)s))
                    {
                        retAtol = atol(valuePtr);
                        if((retAtol > PDCP_PEER_SDU_HEAD_LEN) && (retAtol < 65536))   //(PDCP_PEER_SDU_HEAD_LEN + 1) ~ 65535
                        {
                            cfgFileSt[i].sndPkgSize = retAtol;
                            printf("cfg read [snd_pkg_size%d]=[%d]\n", i, cfgFileSt[i].sndPkgSize);
                            cfgFileSt[i].itemNum += 1;
                            break;
                        }
                        else
                        {
                            printf("[snd_pkg_size%d] invalid!\n", i);
                            fclose(fp);
                            return -1;
                        }
                    }
                    else if(0 == strcmp(paramPtr, (char *)n))
                    {
                        retAtol = atol(valuePtr);
                        if((retAtol >  0) && (retAtol < 65536))
                        {
                            cfgFileSt[i].sndPkgNum = retAtol;
                            printf("cfg read [snd_pkg_num%d]=[%d]\n", i, cfgFileSt[i].sndPkgNum);
                            cfgFileSt[i].itemNum += 1;
                            break;
                        }
                        else
                        {
                            printf("[snd_pkg_num%d] invalid!\n", i);
                            fclose(fp);
                            return -1;
                        }
                    }
                    else if(0 == strcmp(paramPtr, (char *)t))
                    {
                        retAtol = atol(valuePtr);
                        if(TRUE == isInSet(retAtol, (UINT16 *)tmsArr, sizeof(tmsArr)))
                        {
                            cfgFileSt[i].sndIntervalTime = retAtol;
                            printf("cfg read [snd_interval_time%d]=[%d]\n", i, cfgFileSt[i].sndIntervalTime);
                            cfgFileSt[i].itemNum += 1;
                            break;
                        }
                        else
                        {
                            printf("[snd_interval_time%d] invalid!\n", i);
                            fclose(fp);
                            return -1;
                        }
                    }
                    else if(0 == strcmp(paramPtr, (char *)r))
                    {
                        retAtol = atol(valuePtr);
                        if(retAtol >= 1 && retAtol <= 8)
                        {
                            cfgFileSt[i].sndPkgPri = retAtol;
                            printf("cfg read [snd_pkg_pri%d]=[%d] \n", i, cfgFileSt[i].sndPkgPri);
                            cfgFileSt[i].itemNum += 1;
                            break;
                        }
                        else
                        {
                            printf("[snd_pkg_pri%d] invalid! \n", i);
                            fclose(fp);
                            return -1;
                        }
                    }
                    else if(0 == strcmp(paramPtr, (char *)p))
                    {
                        retAtol = atol(valuePtr);
                        if(retAtol >= -30 && retAtol <= 23)
                        {
                            cfgFileSt[i].power = retAtol;
                            printf("cfg read [power%d]=[%d] \n\n", i, cfgFileSt[i].power); //每组配置添加一个换行分隔
                            cfgFileSt[i].itemNum += 1;
                            break;
                        }
                        else
                        {
                            printf("[power%d] invalid!\n", i);
                            fclose(fp);
                            return -1;
                        }
                    }

                }
            }
        }
        printf("end read config file. \r\n");
        // close opend file
        fclose(fp);

        retVal = 0; //retVal在这里必须清0（在文件尾端cfgGetParamAndValue可能处理空格或注释失败！）

        //判断有无autoRunState，作为是否自动的直接依据
        if(1 != autoRunState)
        {
            //自动运行失败！
            printf("no set autorun_state!\n");
            retVal = -1;
        }
        else
        {
            //若autoRunState=1
            //首先判断每个group是否完备
            for(i = 0; i < DEFAULT_CONFIG_MAX_NUM; ++i)
            {
                if( 0 != cfgFileSt[i].itemNum && 8 != cfgFileSt[i].itemNum)
                {
                    printf("the cfgFileSt[%d] is not complete! \n", i);
                    return -1; //存在不完备的group，直接返回-1
                }
                else if(8 == cfgFileSt[i].itemNum)
                {
                    cfgAvailNum += 1; //总共有效的配置的group数量
                }
            }

            //所有的组均完备，但总配置的group数量为0
            if(0 == cfgAvailNum)
            {
                printf("the cfgAvailNum is 0! \n");
                return -1;
            }

            printf("autoRunState=%d, cfgAvailNum=%d, ready for trans! \n" ,autoRunState ,cfgAvailNum);
            retVal = 0;  //不直接return！
        }
        return retVal;
    }
    else
    {
        printf("open config file fail.\r\n");
        retVal = -1;

    }

    // return error code
    return retVal;
}

SINT32 cfgInitialize()
{
    // clear config-struct
    int i;
    for(i = 0; i < DEFAULT_CONFIG_MAX_NUM; ++i)
    {
        memset(&(cfgFileSt[i]), 0x0, sizeof(configFileSt));
    }
    return 0;
}

int findPidByName(char* procName, int* foundPid, int idCount)
{
    DIR *dir;
    struct dirent *d_ptr;
    int pid_temp, i;

    FILE *status_file;
    char filename_temp[128];
    char buffer[256];
    char name[128];

    if ((!procName) || (strlen(procName) == 0) || (idCount < 2))
    {
        printf("findPidByName invalid parameter.\n");
        return -1;
    }

    i = 0;
    foundPid[0] = 0;  //同一个程序可能有多个进程在运行，pid各不相同

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if (!dir)
    {
        printf("cannot open /proc");
        return -1;
    }

    /* Walk through the directory. */
    while ((d_ptr = readdir(dir)) != NULL)
    {
        /* Must skip ".." since that is outside /proc */
        if (strcmp(d_ptr->d_name, "..") == 0 || strcmp(d_ptr->d_name, ".") == 0)
            continue;

        //读到的不是文件夹，跳过
        if(DT_DIR != d_ptr->d_type)
            continue;

        /* See if this is a process */
        if ((pid_temp = atoi(d_ptr->d_name)) == 0)
            continue; //atoi失败返回0

        //该文件（夹）是一个进程的pid
        memset(filename_temp, 0 ,sizeof(filename_temp));
        sprintf(filename_temp, "/proc/%s/status", d_ptr->d_name);

        if (! (status_file = fopen(filename_temp, "r")) )
            continue;

        if (fgets(buffer, 256 -1, status_file) == NULL) //读入一行到buffer中
        {
            fclose(status_file);
            continue;
        }

        fclose(status_file);

         //得到进程id
        /* Buffer should contain a string like "Name:   binary_name" */
        sscanf(buffer, "%*s %s", name);
        if (strcmp(name, procName) == 0)   //匹配到了进程名字
        {
            foundPid[i] = pid_temp; //这里的pid为前面文件夹的名字（也可以用*(foundPid)）
            //打印每个找到的pid
            //printf("foundPid[i] = %d\n", foundPid[i]);
            i++;

            if (i >= (idCount - 1)) //idCount默认为最大的进程数！
            {
                break;
            }
        }

    }
    foundPid[i] = 0; //把最后一个未"使用"的pid元组置0
    closedir(dir);

    return  0;
}


SINT32 isProgressRunning(char *progName, int *pidArr)
{
    int rtn = 0;
    int i = 0;
    //int pid_t[64]; //64代表最大程序最大运行的进程数
    int pid_temp = 0;

    if ((!progName) || (strlen(progName) == 0))
    {
        printf("isProgressRunning invalid parameter.\n");
        return -1;
    }

    // get pid
    rtn = findPidByName(progName, pidArr, 64);
    if (rtn != 0)
    {
        printf("cal find_pid_by_name, return error.\n") ;  //若根据名字找不到pid，说明前面自启动是失败的。
        return -1;
    }

    for(i = 0; pidArr[i] != 0; ++i)
    {
        pid_temp = pidArr[i];
        printf("process(%s) pid is: %d, i:%d.\n", progName, pid_temp, i);

        if (pid_temp <= 0)
        {
            printf("getted pid(%d) is not valid.\n", pid_temp);
            return -1;
        }
    }

    return  0; //找到pid，成功返回！
}














int main()
{
    int retVal = 0;
    int pid_t[64]; //64代表最大程序最大运行的进程数
    int i = 0;

#if 1
    //首先判断是否在运行，若是则SIGKILL
    if(0 == isProgressRunning(DEFAULT_PROGRESS_NAME, &(pid_t[0])))
    {
        printf("4444444isProgressRunning return succ!\n");
        for(i = 0; pid_t[i] != 0; ++i)
        {
            if(pid_t[i] != getpid())
            {
                printf("will kill pid: [%d] \n", pid_t[i]);
                retVal = kill(pid_t[i], SIGKILL);
                if (retVal != 0)
                {
                    printf("send signal SIGKILL error:%d-%s. pid:%d. \n", errno, strerror(errno), pid_t[i]);
                    return -1;
                }
                else
                {
                    printf("send signal SIGKILL OK. pid:%d.\n", pid_t[i]);
                }
            }
        }
    }
#endif

    cfgInitialize();

    retVal = cfgFileReadAndCheck(mConfigFileA);
    if(0 != retVal)
    {
        printf("Autorun failed!\n");
        return retVal;
    }
    else
    {
        printf("Autorun success, and will trans data!\n");  //实际场景中，可以置起一个全局标志
    }

    //for test!
    while(1)
    {
        usleep(2*1000*1000);
    }

    return retVal;
}
