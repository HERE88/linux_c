#define _LOG_UTIL_C

/*---------------------------- include head file -----------------------------*/

#include "sys_file.h"
#include "log_util.h"

// message buffer
static char msg[MAX_PRINT_INFO_LEN + 1] = {0};

/**
 * @brief :call android's function to output during android system, or 
 * call print to output during other system.
 * 
 * @param format :In, same as printf(...)
 * @param ... :
 */
void lc_logcat_error( CHAR *format, ...)
{
    va_list sa;

    va_start(sa, format);
    // clear message buffer
    memset(msg, 0, sizeof(msg));
    vsnprintf(msg,
              MAX_PRINT_INFO_LEN,
              (char *) format,
              sa);
    va_end(sa);

#ifdef OPT_FOR_ANDROID
    ALOGE("%s", msg);               // call android's function to output
#else
    printf("%s\n", msg);            // call printf to output
#endif
    return;
}

/**
 * @brief :call android's function to output during android system, or 
 * call print to output during other system.
 * 
 * @param format :In, same as printf(...)
 * @param ... :
 */
void lc_logcat_warning( CHAR *format, ...)
{
    va_list sa;

    va_start(sa, format);
    // clear message buffer
    memset(msg, 0, sizeof(msg));
    vsnprintf(msg,
              MAX_PRINT_INFO_LEN,
              (char *) format,
              sa);
    va_end(sa);

#ifdef OPT_FOR_ANDROID
    ALOGW("%s", msg);               // call android's function to output
#else
    printf("%s\n", msg);            // call printf to output
#endif
    return;
}

/**
 * @brief :call android's function to output during android system, or 
 * call print to output during other system.
 * 
 * @param format :In, same as printf(...)
 * @param ... :
 */
void lc_logcat_info( CHAR *format, ...)
{
    va_list sa;

    va_start(sa, format);
    // clear message buffer
    memset(msg, 0, sizeof(msg));
    vsnprintf(msg,
              MAX_PRINT_INFO_LEN,
              (char *) format,
              sa);
    va_end(sa);
#ifdef OPT_FOR_ANDROID
    ALOGD("%s", msg);                  // call android's function to output
#else
    printf("%s\n", msg);                // call printf to output
#endif
    return;
}


#undef _LOG_UTIL_C

