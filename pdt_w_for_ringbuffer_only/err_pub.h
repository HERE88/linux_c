#ifndef _ERR_PUB_H
#define _ERR_PUB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    ERR_NONE                                                            = 0,       /* No error */

    ERR_BEGIN                                                           = -100,
    ERR_UNKNOWN                                                         = -100,    /* Unknown error */
    ERR_INVALID_PARAM                                                   = -101,    /* Invalid parameter */
    ERR_PARAM_POINTER_NULL                                              = -102,    /* Parameter pointer is null */
    ERR_INVALID_PARAM_VALUE                                             = -103,    /* Invalid parameter value */
    ERR_NOT_FOUND                                                       = -104,    /* Not found */
    ERR_NOT_READY                                                       = -111,    /* Not ready */
    ERR_TIMEOUT                                                         = -113,    /* Timeout */
    ERR_UNKNOWN_OPER                                                    = -116,    /* Unknown operation */
    ERR_EXCEED_MAX_SIZE                                                 = -117,    /* Exceed max size */
    ERR_CALL_C_FUNC                                                     = -118,    /* Call C function unsuccessfully */
    ERR_WRITE_NOT_COMPLETEDLY                                           = -119,
    ERR_NO_FREE_SPACE                                                   = -120,    /* No free space */
}
LOG_CTRL_ERR_CODE_EN;


#ifdef __cplusplus
}
#endif

#endif /* _ERR_PUB_H */

