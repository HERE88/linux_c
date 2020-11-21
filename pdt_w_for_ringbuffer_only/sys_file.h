#ifndef _SYS_FILE_H_
#define _SYS_FILE_H_

/*-------------------------- include external file ---------------------------*/

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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef NULLPTR
#define NULLPTR                                 (void *)0
#endif

#ifndef NULL
#define NULL                                    0
#endif

#ifndef TRUE
#define TRUE                                    1
#endif
#ifndef FALSE
#define FALSE                                   0
#endif

#ifndef ENUM
#define ENUM                                    enum
#endif


#ifndef CONST
#define CONST                                   const
#endif

#ifndef EXTERN
#define EXTERN                                  extern
#endif


/*----------------------- constant and type definition -----------------------*/

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

#ifdef DISABLE_STATIC

#define STATIC

#else /* #ifdef DISABLE_STATIC */

#ifndef STATIC
#define STATIC                                  static
#endif

#endif /* #ifdef DISABLE_STATIC */


#ifdef __cplusplus
}
#endif


#endif /* _SYS_FILE_H_ */

