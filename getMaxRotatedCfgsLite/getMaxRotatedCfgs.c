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

#include <math.h>

#include <mqueue.h>

//#include "apue.h"
#include <pthread.h>



typedef signed  int                             SINT32;
typedef unsigned  int                           UINT32;

char * g_cfgFileName = "test.cfg";
char * g_cfgFileName2 = "test2.cfg";

int g_maxRotatedCfgs = 0;

#if 0
const char * CFG_FILE_PATH= "/system/etc/";
#else
const char * CFG_FILE_PATH= "/home/wanghaitao/tmp/linux_C/test_folder/";
#endif

#define MAX_CFG_FILE_NUM  64


static void rotateCfgsEx(char * cfgFileName);
static void getMaxRotatedCfgs(char *cfgFileName, char *cfgFileName2);
static void getMaxRotatedCfgsLite(char *cfgFileName);


static void getMaxRotatedCfgs(char *cfgFileName, char *cfgFileName2)
{
    DIR *dir;
    struct dirent *dirp;
    int nMax[2];
    int nFiles[2][MAX_CFG_FILE_NUM];
    int i, j;
    int suffixN;

    printf("getMaxRotatedCfgs called!\n");

#if 1
    memset(nMax, 0, sizeof(nMax));  //iniaialize to 0
    memset(nFiles, 0, sizeof(nFiles));  //iniaialize to 0

    dir = opendir(CFG_FILE_PATH);
    if(dir == NULL)
    {
        printf("opendir error:%d, %s\n", errno, strerror(errno));
        return;
    }
    while((dirp = readdir(dir)) != NULL)
    {
        char *fileNameTmp;
        if(dirp->d_type == DT_DIR)
            continue;

        if(strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0)
            continue;

        if(strstr(dirp->d_name, cfgFileName) != NULL)
        {
            asprintf(&fileNameTmp, "%s", dirp->d_name);
            suffixN = atoi(fileNameTmp + strlen(cfgFileName));
            free(fileNameTmp);
            printf("suffixN=%d\n", suffixN);
            if(suffixN >= MAX_CFG_FILE_NUM)
                continue;
            nFiles[0][suffixN] = 1;
        }
        else if(strstr(dirp->d_name, cfgFileName2) != NULL)
        {
            asprintf(&fileNameTmp, "%s", dirp->d_name);
            suffixN = atoi(fileNameTmp + strlen(cfgFileName2));
            free(fileNameTmp);
            printf("2suffixN=%d\n", suffixN);
            if(suffixN >= MAX_CFG_FILE_NUM)
                continue;
            nFiles[1][suffixN] = 1;
        }
    }

    for(i = 0; i < 2; ++i)
    {
        for(j = 0; j < MAX_CFG_FILE_NUM; ++j)
        {
            if(nFiles[i][j] != 0)
                nMax[i] += 1;
            else
            {
                printf("nMax[%d]=%d\n", i, nMax[i]);
                break;
            }
        }
    }

    g_maxRotatedCfgs = (nMax[0] < nMax[1]) ? nMax[0] : nMax[1];

#endif

    //g_maxRotatedCfgs = 3;
    printf("g_maxRotatedCfgs4=%d\n", g_maxRotatedCfgs);

    closedir(dir);
}

static void getMaxRotatedCfgsLite(char *cfgFileName)
{
    DIR *dir;
    struct dirent *dirp;
    int nMax = 0;
    int nFiles[MAX_CFG_FILE_NUM];
    int j;
    int suffixN;

    printf("getMaxRotatedCfgsLite called.\n");

    memset(nFiles, 0, sizeof(nFiles));

    dir = opendir(CFG_FILE_PATH);
    if(dir == NULL)
    {
        printf("opendir error:%d, %s\n", errno, strerror(errno));
        return;
    }
    while((dirp = readdir(dir)) != NULL)
    {
        char *fileNameTmp;
        if(dirp->d_type == DT_DIR)
            continue;

        if(strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0)
            continue;

        if(strstr(dirp->d_name, cfgFileName) != NULL)
        {
            asprintf(&fileNameTmp, "%s", dirp->d_name);
            suffixN = atoi(fileNameTmp + strlen(cfgFileName));
            free(fileNameTmp);
            //printf("suffixN=%d\n", suffixN);
            if(suffixN >= MAX_CFG_FILE_NUM)
                continue;
            nFiles[suffixN] = 1;
        }
    }

    for(j = 0; j < MAX_CFG_FILE_NUM; ++j)
    {
        if(nFiles[j] != 0)
            nMax += 1;
        else
        {
            //printf("nMax[%d]=%d\n", i, nMax[i]);
            break;
        }
    }

    g_maxRotatedCfgs = nMax;

    //g_maxRotatedCfgs = 3;
    printf("g_maxRotatedCfgsLite=%d\n", g_maxRotatedCfgs);

    closedir(dir);
}

static void rotateCfgsEx(char * cfgFileName)
{
    int err;
    int i;

    // Can't rotate cfgs if NULL
    if (cfgFileName == NULL)
    {
        return;
    }

    for (i = 0 ; i <= g_maxRotatedCfgs ; i++)
    {
        char *file0, *file1;

        if(i == 0)
        {
            asprintf(&file1, "%s", cfgFileName);
            asprintf(&file0, "%s%s", cfgFileName, "temp");
        }
        else if(i == 1)
        {
            asprintf(&file1, "%s%d", cfgFileName, i);
            asprintf(&file0, "%s", cfgFileName);
        }
        else if(i == g_maxRotatedCfgs)
        {
            asprintf(&file1, "%s%s", cfgFileName, "temp");
            asprintf(&file0, "%s%d", cfgFileName, g_maxRotatedCfgs - 1);
        }
        else
        {
            asprintf(&file1, "%s%d", cfgFileName, i);
            asprintf(&file0, "%s%d", cfgFileName, i - 1);
        }

        if (!file0 || !file1)
        {
            printf("rotateCfgsEx error1\n");
            break;
        }

        err = rename(file1, file0);  //file1->file0

        if (err < 0 && errno != ENOENT)
        {
            printf("rename error2\n");
        }

        free(file1);
        free(file0);
    }

}
int main()
{
    //getMaxRotatedCfgs(g_cfgFileName, g_cfgFileName2);
    getMaxRotatedCfgsLite(g_cfgFileName2);
    //rotateCfgsEx(g_cfgFileName);

    return 0;
}
