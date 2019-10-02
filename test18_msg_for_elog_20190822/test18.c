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


#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>


#include "rio_socket.h"  //wang

typedef void                                    VOID;
typedef char                                    CHAR;
typedef signed char                             BOOL;

typedef signed char                             SINT8;
typedef signed short                            SINT16;
typedef signed  int                             SINT32;
typedef signed long long                        SINT64;

typedef unsigned char                           UINT8;
typedef unsigned short                          UINT16;
typedef unsigned  int                           UINT32;
typedef unsigned long long                      UINT64;

//#define CONST_FILTER_FILE_NAME "/system/etc/msg_sock.txt"

#pragma pack (1)

/* LTE HLS MSG HEADER for standard interface */
typedef  struct _ELOG_HLS_LTE_MSG_HEAD_
{
    /* Tdiag_ext_f364 (DIAG: diag_type.h) */
    UINT16  segment_head;  //0xF3 64

    /* Tdiag_ext_msg_ver (DIAG: diag_type.h) */
    UINT16  segment_length; //length of this primitive, include param+peer, but not include the 0xF364(2 bytes).
    UINT8   interface_flag;
    UINT8   interface_version;
    UINT16  message_sn;  //cplogctrl <-> tt

    /* Tdiag_ext_msg_head (DIAG: diag_type.h) */
    UINT16  temp_total_msg_len;  //length form here byte, include param+peer.
    UINT16  ucDiagId;
    UINT16  ucWaveId;

    UINT8   src_task_id;
    UINT8   src_process_id;
    UINT8   dest_task_id;
    UINT8   dest_process_id;
    UINT8   ucUeId;
    UINT8   ucResv;
    UINT32  sap_id;
    UINT32  prim_id;
    UINT32  timestamp;

    /* part of msg head which are newly added: */
    UINT32  ulFn;
    UINT16  usOffset;

    UINT16  param_len;
    UINT16  usPeerLen;
    UINT16  usfreeHeaderlen;
} ELOG_HLS_MSG_LTE_HEAD;

typedef struct _P_DIAG_TT_INFO_CNF_param
{
    UINT16 diag_id;
    UINT16 wave_num;
    UINT8  diag_name[16];
    UINT8  wave_name[1024];
}P_DIAG_TT_INFO_CNF_param;

typedef struct _P_DIAG_TT_FILTER_REQ_param
{
    UINT32 sap_num;
    UINT32 sap[128];
}P_DIAG_TT_FILTER_REQ_param;

#pragma pack ()


rio_t rio_buf;  //buffer

int main()
{
    UINT8 rcv_buf[4096];
    UINT8 snd_buf[4096];
    UINT32 rd_cnt;

    ELOG_HLS_MSG_LTE_HEAD *msg_rcv = (ELOG_HLS_MSG_LTE_HEAD *)rcv_buf;
    ELOG_HLS_MSG_LTE_HEAD *msg_snd = (ELOG_HLS_MSG_LTE_HEAD *)snd_buf;

    int listen_fd, connect_fd;

    int tmp;

    P_DIAG_TT_INFO_CNF_param *tmp_p = NULL;
    P_DIAG_TT_FILTER_REQ_param *tmp_p2 = NULL;

    listen_fd = open_listenfd("9998");
    if(listen_fd < 0)
    {
        printf("open_listenfd error\n");
        exit(1);
    }

    connect_fd = accept(listen_fd, 0, 0);
    if(connect_fd < 0)
    {
        printf("accept error\n");
        exit(1);
    }

    rio_readinitb(&rio_buf, connect_fd);

    /* process each received msg */
    while(1)
    {
        memset(rcv_buf, 0, sizeof(rcv_buf));
        rd_cnt = 0;

        //read
        if(rio_readnb(&rio_buf, rcv_buf, sizeof(ELOG_HLS_MSG_LTE_HEAD)) != sizeof(ELOG_HLS_MSG_LTE_HEAD))
        {
            printf("rio_readnb error\n");
            exit(1);
        }
        rd_cnt += sizeof(ELOG_HLS_MSG_LTE_HEAD);

        tmp = ntohs(msg_rcv->segment_length) + 2 - sizeof(ELOG_HLS_MSG_LTE_HEAD);
        if(tmp > 0)
        {
            if(rio_readnb(&rio_buf, rcv_buf + rd_cnt,  tmp) != tmp)
            {
                printf("rio_readnb2 error\n");
                exit(1);
            }
        }

        switch (ntohl(msg_rcv->prim_id))
        {
            case 12102:  //P_CPLOGCTRL_DIAG_ACCESS_IND
                printf("received P_CPLOGCTRL_DIAG_ACCESS_IND(12102)\n");
                break;

            case 12101:  //P_CPLOGCTRL_DIAG_ACCESS_CNF
                printf("received P_CPLOGCTRL_DIAG_ACCESS_CNF(12101)\n");
                break;

            case 12108:  //P_CPLOGCTRL_DIAG_START_IND
                printf("received P_CPLOGCTRL_DIAG_START_IND(12108)\n");
                /* send P_DIAG_CPLOGCTRL_ACCESS_REQ */
                memset(snd_buf, 0, sizeof(snd_buf));
                msg_snd->segment_head = 0x64F3;
                msg_snd->segment_length = htons(sizeof(ELOG_HLS_MSG_LTE_HEAD) - 2);
                msg_snd->prim_id = htonl(12100);  //P_DIAG_CPLOGCTRL_ACCESS_REQ
                msg_snd->interface_version = 0x02;
                if(rio_writen(rio_buf.rio_fd, snd_buf, sizeof(ELOG_HLS_MSG_LTE_HEAD)) != sizeof(ELOG_HLS_MSG_LTE_HEAD))
                {
                    printf("rio_writen error\n");
                    exit(1);
                }
                break;

            case 12107:  //P_CPLOGCTRL_DIAG_INFO_REQ
                printf("received P_CPLOGCTRL_DIAG_INFO_REQ(12107)\n");
                memset(snd_buf, 0, sizeof(snd_buf));
                msg_snd->segment_head = 0x64F3;
                msg_snd->src_task_id = 10;
                msg_snd->src_process_id = 2;
                msg_snd->dest_task_id = 15;
                msg_snd->dest_process_id = 0;
                msg_snd->sap_id = htonl(13002);
                msg_snd->prim_id = htonl(12202);  //P_DIAG_TT_INFO_CNF
                msg_snd->interface_version = 0x02;
                msg_snd->usPeerLen = htons(0);
                msg_snd->usfreeHeaderlen = htons(0);
                msg_snd->ucDiagId = htons(1);

                tmp_p = (P_DIAG_TT_INFO_CNF_param *)(snd_buf + sizeof(ELOG_HLS_MSG_LTE_HEAD));
                tmp_p->diag_id = htons(1);
                tmp_p->wave_num = htons(1);
                snprintf(tmp_p->diag_name, sizeof(tmp_p->diag_name), "%s", "DIAG1-WANG");
                snprintf(tmp_p->wave_name, sizeof(tmp_p->wave_name), "%s", "ZSCA1-WANG");
                msg_snd->param_len = htons(sizeof(P_DIAG_TT_INFO_CNF_param));
                msg_snd->temp_total_msg_len = htons(sizeof(ELOG_HLS_MSG_LTE_HEAD) - 8 + ntohs(msg_snd->param_len));
                msg_snd->segment_length = htons(sizeof(ELOG_HLS_MSG_LTE_HEAD) - 2 + ntohs(msg_snd->param_len));

                if(rio_writen(rio_buf.rio_fd, snd_buf, (sizeof(ELOG_HLS_MSG_LTE_HEAD) + sizeof(P_DIAG_TT_INFO_CNF_param)))
                    != (sizeof(ELOG_HLS_MSG_LTE_HEAD) + sizeof(P_DIAG_TT_INFO_CNF_param)))
                {
                    printf("rio_writen error\n");
                    exit(1);
                }
                break;

            case 12106: //P_CPLOGCTRL_DIAG_FILTER_CHANGE_IND
                printf("received P_CPLOGCTRL_DIAG_FILTER_CHANGE_IND(12106)\n");
                memset(snd_buf, 0, sizeof(snd_buf));
                msg_snd->segment_head = 0x64F3;
                //msg_snd->segment_length = htons(sizeof(ELOG_HLS_MSG_LTE_HEAD) - 2);
                msg_snd->src_task_id = 10;
                msg_snd->src_process_id = 2;
                msg_snd->dest_task_id = 15;
                msg_snd->dest_process_id = 0;
                msg_snd->sap_id = htonl(13002);
                msg_snd->prim_id = htonl(12200);  //P_DIAG_TT_FILTER_REQ
                msg_snd->interface_version = 0x02;
                msg_snd->usPeerLen = htons(0);
                msg_snd->usfreeHeaderlen = htons(0);
                msg_snd->ucDiagId = htons(1);

                tmp_p2 = (P_DIAG_TT_FILTER_REQ_param *)(snd_buf + sizeof(ELOG_HLS_MSG_LTE_HEAD));
                tmp_p2->sap_num = htonl(1);  //sap_num*2 is the nums of sap id.
                tmp_p2->sap[0] = htonl(0);
                tmp_p2->sap[1] = htonl(0x0FFFFFFF);
                msg_snd->param_len = htons((ntohl(tmp_p2->sap_num)*2 + 1)*sizeof(UINT32));
                msg_snd->temp_total_msg_len = htons(sizeof(ELOG_HLS_MSG_LTE_HEAD) - 8 + ntohs(msg_snd->param_len));
                msg_snd->segment_length = htons(sizeof(ELOG_HLS_MSG_LTE_HEAD) - 2 + ntohs(msg_snd->param_len));

                if(rio_writen(rio_buf.rio_fd, snd_buf, (sizeof(ELOG_HLS_MSG_LTE_HEAD) + ntohs(msg_snd->param_len)))
                    != (sizeof(ELOG_HLS_MSG_LTE_HEAD) + ntohs(msg_snd->param_len)))
                {
                    printf("rio_writen error\n");
                    exit(1);
                }
                break;

            case 12201:
                printf("received P_TT_DIAG_FILTER_CNF(12201)\n");
#if 1
                //here send some msg for test store in elog files.
                memset(snd_buf, 0, sizeof(snd_buf));
                msg_snd->segment_head = 0x64F3;
                msg_snd->segment_length = htons(sizeof(ELOG_HLS_MSG_LTE_HEAD) - 2);
                msg_snd->prim_id = htonl(12206);  //P_OSA_TT_PRINT_IND
                msg_snd->interface_version = 0x02;
                if(rio_writen(rio_buf.rio_fd, snd_buf, sizeof(ELOG_HLS_MSG_LTE_HEAD)) != sizeof(ELOG_HLS_MSG_LTE_HEAD))
                {
                    printf("rio_writen error\n");
                    exit(1);
                }
#endif
                break;


        }



    }


    sleep(10);


    return 0;


}
