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
#include <sys/time.h>


typedef int SINT32;
typedef unsigned int UINT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;

#define LOGCAT_DEBUG(...)        printf(__VA_ARGS__)

SINT32 create_folder_if_not_exist(const char *folder_ptr);
SINT32 mkdir_recur_ex(const char *path, mode_t mode, UINT32 try_count, UINT32 try_interval);
SINT32 mkdir_recur(const char *path, mode_t mode);

SINT32 mkdir_recur(const char *path, mode_t mode)
{
    SINT32  ret_val = 0;            // error code
    char opath[256];
    char *p;
    size_t len;
    int index = 0;

    strncpy(opath, path, sizeof(opath));
    opath[sizeof(opath) - 1] = '\0';
    len = strlen(opath);
    if (len == 0)
        return ret_val;
    else if (opath[len - 1] == '/')
        opath[len - 1] = '\0';

    if (opath[0] == '/')  //if given the absolute path, which begin with '/'.
        index++;;

    for(p = opath + index; *p; p++)
    {
        if (*p == '/')
        {
            *p = '\0';
            if (access(opath, F_OK))
            {
                ret_val = mkdir(opath, mode);
                if (ret_val != 0)
                {
                    LOGCAT_DEBUG("create folder(%s) error:%d-%s\n", opath, errno, strerror(errno));
                    return -1;
                }
            }
            *p = '/';  //resume the '/'
        }
    }

    if(access(opath, F_OK))         /* if path is not terminated with '/' */
    {
        ret_val = mkdir(opath, mode);
        if (ret_val != 0)
        {
            LOGCAT_DEBUG("create folder(%s) error:%d-%s\n", opath, errno, strerror(errno));
            return -1;
        }
    }

    return 0;
}

SINT32 mkdir_recur_ex(const char *path, mode_t mode, UINT32 try_count, UINT32 try_interval)
{
    SINT32  ret_val = 0;
    UINT32  i = 0;

    if((path == NULL) || (0 == strlen(path)))
    {
        LOGCAT_DEBUG("mkdir_recur_ex invalide parameter\n");
        return -1;
    }

    do
    {
        LOGCAT_DEBUG("(%d/%d)try to make dir \"%s\"\n", i + 1, try_count, path);
        ret_val = mkdir_recur(path, mode);
        if(ret_val == 0)
            break;

        LOGCAT_DEBUG("make dir \"%s\" error: %d - %s\n", path, errno, strerror(errno));
        i++;
        sleep(try_interval);
    } while(i < try_count);

    if(ret_val == 0)
    {
        LOGCAT_DEBUG("make dir \"%s\" successfully\n", path);
    }
    else
    {
        ret_val = -1;
        LOGCAT_DEBUG("make dir \"%s\" fail: %d - %s, tried-times:%d\n", path, errno, strerror(errno), try_count);
    }

    return ret_val;
}

SINT32 create_folder_if_not_exist(const char *folder_ptr)
{
    SINT32  ret_val = 0;            // error code

    // check whether the path exist or not
    if(0 != access(folder_ptr, F_OK) )
    {
        ret_val = mkdir_recur_ex(folder_ptr, S_IRWXU | S_IRWXG | S_IRWXO, 30, 10);
        LOGCAT_DEBUG("create folder(%s), return:%d\n", folder_ptr, ret_val);
    }
    else
    {
        LOGCAT_DEBUG("access path(%s) success\n", folder_ptr);
    }

    return ret_val;
}

int main()
{
    int ret = 0;
    char * test = "/home/wanghaitao/elog/test/test1/test2";

    ret = create_folder_if_not_exist(test);

    if(ret != 0)
        LOGCAT_DEBUG("create_folder_if_not_exist error\n");
    else
        LOGCAT_DEBUG("create_folder_if_not_exist success!\n");

    return 0;
}