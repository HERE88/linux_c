/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : Locking semaphore for hardware sharing
--
------------------------------------------------------------------------------*/
 /*******************************************************************************
* CR History:
********************************************************************************
*  1.0.0     EDEN_Req00000001       yangshudan     2012-2-26    add ON2 Codec  function *
*  1.0.1     EDEN_Req00000003   zhangziming    2012-4-11   add log stream print and mutex protect *
*********************************************************************************/

/* FIX EDEN_Req00000001 BEGIN DATE:2012-2-26 AUTHOR NAME YANGSHUDAN */
/*no sys/ipc.h, sys/sem.h, sys/types.h*/
//#include <sys/ipc.h> 
#include <linux/ipc.h> 
//#include <sys/sem.h> 
#include <linux/sem.h> 
//#include <sys/types.h>
#include <linux/types.h>
/* FIX EDEN_Req00000001 END DATE:2012-2-26 AUTHOR NAME YANGSHUDAN */
/* FIX EDEN_Req00000003 BEGIN DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */
#include <sys/syscall.h>
/* FIX EDEN_Req00000003 END DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */
#include <errno.h>

#include "dwl_linux_lock.h"


/* FIX EDEN_Req00000001 BEGIN DATE:2012-2-26 AUTHOR NAME YANGSHUDAN */
/*semun is redefined*/
#if 0 
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};
#endif
/* FIX EDEN_Req00000001 END DATE:2012-2-26 AUTHOR NAME YANGSHUDAN */


/* Obtain a binary semaphore's ID, allocating if necessary.  */

int binary_semaphore_allocation(key_t key, int sem_flags)
{

/* FIX EDEN_Req00000003 BEGIN DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */
/*semget is undefined*/
    //return semget(key, 2, sem_flags);
    return syscall(__NR_semget, key, 2, sem_flags);
/* FIX EDEN_Req00000003 END DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */
}

/* Deallocate a binary semaphore.  All users must have finished their
   use.  Returns -1 on failure.  */

int binary_semaphore_deallocate(int semid)
{
    union semun ignored_argument;


/* FIX EDEN_Req00000003 BEGIN DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */
/*function undefined*/
    //return sys_semctl(semid, 1, IPC_RMID, ignored_argument);
    return syscall(__NR_semctl, semid, 1, IPC_RMID, ignored_argument);
/* FIX EDEN_Req00000003 END DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */

}

/* Wait on a binary semaphore.  Block until the semaphore value is
   positive, then decrement it by one.  */

int binary_semaphore_wait(int semid, int sem_num)
{
    struct sembuf operations[1];
    int ret;
    
    /* Use 'sem_num' semaphore from the set.  */
    operations[0].sem_num = sem_num;
    /* Decrement by 1.  */
    operations[0].sem_op = -1;
    /* Permit undo'ing.  */
    operations[0].sem_flg = SEM_UNDO;
    
/* FIX EDEN_Req00000003 BEGIN DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */
    /* signal safe */
    do {
        //ret = semop(semid, operations, 1);
        ret = syscall(__NR_semop, semid, operations, 1);
    } while (ret != 0 && errno == EINTR);
/* FIX EDEN_Req00000003 END DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */

    return ret;
}

/* Post to a binary semaphore: increment its value by one.  This
   returns immediately.  */

int binary_semaphore_post(int semid, int sem_num)
{
    struct sembuf operations[1];
    int ret;
    
    /* Use 'sem_num' semaphore from the set.  */
    operations[0].sem_num = sem_num;
    /* Increment by 1.  */
    operations[0].sem_op = 1;
    /* Permit undo'ing.  */
    operations[0].sem_flg = SEM_UNDO;

/* FIX EDEN_Req00000003 BEGIN DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */
    /* signal safe */
    do {
        //ret = semop(semid, operations, 1);
        ret = syscall(__NR_semop, semid, operations, 1);
    } while (ret != 0 && errno == EINTR);
/* FIX EDEN_Req00000003 END DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */

    return ret;
}

/* Initialize a binary semaphore with a value of one.  */

int binary_semaphore_initialize(int semid)
{
    union semun argument;
    unsigned short values[2];

    values[0] = 1;
    values[1] = 1;
    argument.array = values;
/* FIX EDEN_Req00000003 BEGIN DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */
/*function undefined*/
    //return sys_semctl(semid, 0, SETALL, argument);
    return syscall(__NR_semctl, semid, 0, SETALL, argument);
/* FIX EDEN_Req00000003 END DATE:2012-4-11 AUTHOR NAME ZHANGZIMING */

}
