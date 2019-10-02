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

#include <sys/vfs.h>

typedef int SINT32;
typedef unsigned int UINT32;
typedef unsigned char UINT8;

#define LOGCAT_DEBUG(...)        printf(__VA_ARGS__)

typedef struct
{
    UINT32      m_total;                        /* total size(KB) */
    UINT32      m_used;                         /* used size(KB) */
    UINT32      m_available;                    /* available size(KB) */
} DISK_INFO_ST;

SINT32 get_filesystem_info( const char *fs_ptr, DISK_INFO_ST *info_ptr )
{
    SINT32  ret_val = 0;            // error code

    struct statfs   fs_st;              // file-status struct

    /* Check parameter. */
    if( !fs_ptr || strlen(fs_ptr) == 0 || !info_ptr )
    {
        LOGCAT_DEBUG("get_filesystem_info para error.\n");
        return -1;
    }

    /* Try to get the storage info of the file system. */
    if( 0 == ret_val )
    {
        // clear struct
        memset(info_ptr, 0, sizeof(DISK_INFO_ST));

        /* Try to get the storage info of the file system. */
        ret_val = statfs(fs_ptr, &fs_st);

        if( 0 == ret_val )
        {
            ret_val = 0;

            /* f_bsize : optimal transfer block size(byte size of each block)
             * f_blocks: total data blocks in file system
             * f_bavail: free blocks avail to non-superuser
             * f_bfree : free blocks in file system */
            info_ptr->m_total = (UINT32)(fs_st.f_blocks * fs_st.f_bsize / 1024);
            info_ptr->m_used = (UINT32)((fs_st.f_blocks - fs_st.f_bavail) * fs_st.f_bsize / 1024);
            info_ptr->m_available = (UINT32)(fs_st.f_bavail * fs_st.f_bsize / 1024);

            if (( info_ptr->m_total == 0) || (info_ptr->m_available == 0))
            {
                LOGCAT_DEBUG("file system \"%s\" has not been mounted", fs_ptr);
                ret_val = -1;
            }
        }
        else
        {
            LOGCAT_DEBUG("get_filesystem_info(%s) error:%d-%s \n", fs_ptr, errno, strerror(errno));
            ret_val = -1;
        }
    }

    return ret_val;
}

int main()
{
    int ret = 0, index;
    DISK_INFO_ST test;

    ret = get_filesystem_info("/home/wanghaitao/", &test);
    if(ret != 0)
        LOGCAT_DEBUG("error\n");
    else
    {
        LOGCAT_DEBUG("total size(KB)    :%u \nused  size(KB)    :%u \navailable size(KB):%u\nused percent(%%)   :%.2f\n", test.m_total, test.m_used, test.m_available, (test.m_used*100.0)/test.m_total);
    }

    return 0;
}