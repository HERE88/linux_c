#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


typedef unsigned char                       UINT8;
typedef unsigned short                      UINT16;
typedef unsigned  int                       UINT32;

#define DEFAULT_MAX_ROTATED_LOGS 4

static const char * g_outputFileName = "pc5_msg.log";
// 0 means "no log rotation"
static size_t g_logRotateSizeKBytes = 1*1024;  //每一个输出文件的最大容量
// 0 means "unbounded"
static size_t g_maxRotatedLogs = DEFAULT_MAX_ROTATED_LOGS;  //输出文件最大个数
static int g_outFD = -1;
static size_t g_outByteCount = 0;
//static int g_printBinary = 0;
//static int g_devCount = 0;                              // >1 means multiple

#define MAXLINE       1024
#define SIZE_MAX      10*1024*1024

/*******************************************************************/
static int openLogFile (const char *pathname);
static void setupOutput();
static void rotateLogs();
static int printBinary(void *bufPtr, size_t bufLen);

/*******************************************************************/

static int openLogFile (const char *pathname)
{
    return open(pathname, O_WRONLY | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR);
}

static void setupOutput()
{

    if (g_outputFileName == NULL) {
        g_outFD = STDOUT_FILENO;

    } else {
        g_outFD = openLogFile (g_outputFileName);

        if (g_outFD < 0) {
            perror("couldn't open output file \n");
        }

        struct stat statbuf;
        if (fstat(g_outFD, &statbuf) == -1) {
            close(g_outFD);
            perror("couldn't get output file stat\n");
        }

        if ((size_t) statbuf.st_size > SIZE_MAX || statbuf.st_size < 0) {
            close(g_outFD);
            perror("invalid output file stat\n");
        }

        g_outByteCount = statbuf.st_size;
    }
}

static void rotateLogs()
{
    int err;

    // Can't rotate logs if we're not outputting to a file
    if (g_outputFileName == NULL) {
        return;
    }

    close(g_outFD);

    // Compute the maximum number of digits needed to count up to g_maxRotatedLogs in decimal.
    // eg: g_maxRotatedLogs == 30 -> log10(30) == 1.477 -> maxRotationCountDigits == 2
    int maxRotationCountDigits =
            (g_maxRotatedLogs > 0) ? (int) (floor(log10(g_maxRotatedLogs) + 1)) : 0;

    for (int i = g_maxRotatedLogs ; i > 0 ; i--) {
        char *file0, *file1;

        asprintf(&file1, "%s.%.*d", g_outputFileName, maxRotationCountDigits, i);

        if (i - 1 == 0) {
            asprintf(&file0, "%s", g_outputFileName);
        } else {
            asprintf(&file0, "%s.%.*d", g_outputFileName, maxRotationCountDigits, i - 1);
        }

        if (!file0 || !file1) {
            perror("while rotating log files");
            break;
        }

        err = rename(file0, file1);

        if (err < 0 && errno != ENOENT) {
            perror("while rotating log files");
        }

        free(file1);
        free(file0);
    }

    g_outFD = openLogFile(g_outputFileName);

    if (g_outFD < 0) {
        printf("couldn't open output file \n");
    }

    g_outByteCount = 0;

}

static int printBinary(void *bufPtr, size_t bufLen)
{
    int bytesWritten = 0;
    bytesWritten = write(g_outFD, bufPtr, bufLen);
    if( bytesWritten < 0 )
    {
        printf("write to %s error.\n ", g_outputFileName);
        return -1;
    }

    g_outByteCount += bytesWritten;

    if (g_logRotateSizeKBytes > 0
        && (g_outByteCount / 1024) >= g_logRotateSizeKBytes
    ) {
        rotateLogs();
    }
    return 0;
}

char *getTimeStr(char ** bufPtr)
{
    char msgBuf[64] = {0};
    struct timeval tv;
    struct tm * tm_ptr;

    gettimeofday(&tv, NULL);
    tm_ptr = localtime(&(tv.tv_sec));

    snprintf(msgBuf, sizeof(msgBuf),"%02d-%02d %02d:%02d:%02d.%06d", tm_ptr->tm_mon, tm_ptr->tm_mday,
        tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, tv.tv_usec);

    //printf("msgBuf: %s\n", msgBuf);
    strncpy(*bufPtr, msgBuf, sizeof(msgBuf));

    return (*bufPtr);
}

int main(int argc, char *argv[])
{
    int bytesWritten = 0;
    int err;
    char msgBuf[MAXLINE];
    struct timeval tv;

	char timeStr[64] = {0};
	char *tPtr = timeStr;

	//打印真实时间
	struct tm * tm_ptr;

    setupOutput();
    while (1)
    {
		//time(&t_temp);
	    gettimeofday(&tv, NULL);
		tm_ptr = localtime(&(tv.tv_sec));
        snprintf(msgBuf, sizeof(msgBuf), "[%s]tv_sec =%ld, tv_usec=%ld[haitao]\n", getTimeStr(&tPtr), tv.tv_sec, tv.tv_usec);
        if(0 != printBinary(msgBuf, strlen(msgBuf)))
        {
            printf("printBinary() error!\n");
            return -1;
        }
        usleep(1*1000);
    }
}
