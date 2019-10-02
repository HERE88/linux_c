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

typedef int SINT32;
typedef unsigned int UINT32;
typedef unsigned char UINT8;


#define LOGCAT_INFO(...) printf(__VA_ARGS__)
int log_folder_num = 3;


static int dir_has_content(char *dir_name);
static void sfw_log_backup_log(char *log_path_a);


static int dir_has_content(char *dir_name)
{
    DIR *dir;
    struct dirent *ptr;

    // check parameter's validity
    if(!dir_name)
    {
        LOGCAT_INFO("dir_name is null.\n");
        // parameter is not valid
        return -1;
    }

    dir = opendir(dir_name);
    if(dir == NULL) {
        LOGCAT_INFO("fail to open dir(%s) %d - %s.\n", dir_name, errno, strerror(errno));
        return -1;
    }
    errno = 0;
    while((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name, ".") == 0 || strcmp (ptr->d_name, "..") == 0)
            continue;
        if(ptr->d_type == DT_DIR) {
            // LOGCAT_INFO("the dir name is :%s.\n", ptr->d_name);
            return 1;
        }
        // LOGCAT_INFO("the file name is :%s.\n", ptr->d_name);
        return 1;
    }

    if(errno != 0)
    {
        LOGCAT_INFO("fail to read dir(%s) %d - %s.\n", dir_name, errno, strerror(errno));
        return -1;
    }
    closedir(dir);

    LOGCAT_INFO("the dir(%s) has not content.\n", dir_name);
    return 0;
}

static void sfw_log_backup_log(char *log_path_a)
{
    char  buf_tmp[128] = {0};           // char string pointer
    char  buf_tmp2[128] = {0};          // char string pointer
    char  src_folder[128];  // char string pointer
    UINT8 i = 0;                            // integer variable
    UINT32 f_num;                       // integer variable

    LOGCAT_INFO("will backup folder: [%s]\n", log_path_a);

    strncpy(src_folder, log_path_a, sizeof(src_folder) - 1);
    if('/' == src_folder[strlen(log_path_a) - 1])
    {
        LOGCAT_INFO("log_path_a end with '/'\n");
        src_folder[strlen(log_path_a) - 1] = 0;
    }

    f_num = log_folder_num;  //get from cfg file
    if(f_num >= 1)
    {

        /* delete oldest back folder of "%s-%ld" if exist */
        snprintf(buf_tmp, sizeof(buf_tmp),
                  "rm -r -f %s-%u",
                  src_folder, f_num);

        // call system coammand to delete oldest folder
        system(buf_tmp);

        for(i = f_num; i >= 1; i--)
        {
            snprintf(buf_tmp2, sizeof(buf_tmp2),
                     "%s-%d",
                     src_folder, i);
            if(1 == i)
            {

                // rename old folder to new folder
                rename(src_folder, buf_tmp2);
            }
            else
            {
                snprintf(buf_tmp, sizeof(buf_tmp),
                         "%s-%d",
                         src_folder, (i - 1));

                // rename old folder to new folder
                rename(buf_tmp, buf_tmp2);
            }
        }
    }
}

int main()
{
    int ret = 0;
    char * temp = "/home/wanghaitao/elog_test/test/";

    ret = dir_has_content(temp);
    if(ret == 0)
        printf("\"%s\" no content\n", temp);
    else if(ret == 1)
    {
        printf("\"%s\" has content\n", temp);
        sfw_log_backup_log(temp);
    }
    else if(ret == -1)
        printf("error\n");

    return 0;

}
