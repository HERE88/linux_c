#ifndef _LOG_UTIL_H
#define _LOG_UTIL_H

/*-------------------------- include external file ---------------------------*/

#include "sys_file.h"

#define LOG_TAG         "EMSD_PDT"

/* Module name, only used for log. */
#define LOG_MODULE_NAME    "EMSD_PDT"

#ifdef OPT_FOR_ANDROID
#include <cutils/log.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
 
#define MAX_PRINT_INFO_LEN   1024

void lc_logcat_error( CHAR *format, ...);
void lc_logcat_warning( CHAR *format, ...);
void lc_logcat_info( CHAR *format, ...);

#define LOGCAT_ERR( x )                                            \
            lc_logcat_error x
#define LOGCAT_WARN( x )                                           \
            lc_logcat_warning x
#define LOGCAT_INFO( x )                                           \
            lc_logcat_info x

#define LOG_INFO( x )                                              \
            lc_logcat_info x
            
#define LOG_SYS_ERR( x )                                           \
            lc_logcat_error x

#define LOG_ERR( x )                                               \
            lc_logcat_error x

#ifdef __cplusplus
}
#endif

#endif /* _LOG_UTIL_H */

