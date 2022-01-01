#include <stdio.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <errno.h>

#include <string.h>  // strerror()

#if 0
struct sysinfo {
               long uptime;             /* Seconds since boot */
               unsigned long loads[3];  /* 1, 5, and 15 minute load averages */
               unsigned long totalram;  /* Total usable main memory size */
               unsigned long freeram;   /* Available memory size */
               unsigned long sharedram; /* Amount of shared memory */
               unsigned long bufferram; /* Memory used by buffers */
               unsigned long totalswap; /* Total swap space size */
               unsigned long freeswap;  /* swap space still available */
               unsigned short procs;    /* Number of current processes */
               unsigned long totalhigh; /* Total high memory size */
               unsigned long freehigh;  /* Available high memory size */
               unsigned int mem_unit;   /* Memory unit size in bytes */
               char _f[20-2*sizeof(long)-sizeof(int)]; /* Padding to 64 bytes */
           };
#endif

static int print_system_boot_time()
{
    struct sysinfo info;
    time_t cur_time = 0;
    time_t boot_time = 0;
    struct tm *ptm = NULL;
    if (sysinfo(&info)) {
        fprintf(stderr, "Failed to get sysinfo, errno:%u, reason:%s\n",
            errno, strerror(errno));
        return -1;
    }

    printf("info.uptime=%lu\n", info.uptime);

    time(&cur_time);
    if (cur_time > info.uptime) {
        boot_time = cur_time - info.uptime;
    }
    else {
        boot_time = info.uptime - cur_time;
    }
    ptm = gmtime(&boot_time);
    printf("System boot time: %d-%-d-%d %d:%d:%d\n", ptm->tm_year + 1900,
        ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
   return 0;
}

int main()
{
    if (print_system_boot_time() != 0) {
        return -1;
    }
    return 0;
}
