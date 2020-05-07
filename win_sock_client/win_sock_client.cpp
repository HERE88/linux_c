#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void ErrorHandling(char *message);

typedef char                                    CHAR;

typedef signed char                             SINT8;
typedef signed short                            SINT16;
typedef signed  int                             SINT32;
typedef signed long long                        SINT64;

typedef unsigned char                           UINT8;
typedef unsigned short                          UINT16;
typedef unsigned  int                           UINT32;
typedef unsigned long long                      UINT64;
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


int main(int argc, char * argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	//char opmsg[BUF_SIZE];
	//int result, opndCnt, i;
	SOCKADDR_IN servAdr;

	//wanghaitao
	ELOG_HLS_MSG_LTE_HEAD msg_snd;

	if(argc != 3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket=socket(PF_INET, SOCK_STREAM, 0);
	if(hSocket==INVALID_SOCKET)
		ErrorHandling("socket() error!");

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=inet_addr(argv[1]);
	servAdr.sin_port=htons(atoi(argv[2]));

	memset(&msg_snd, 0, sizeof(msg_snd));
	msg_snd.segment_head = 0x64F3;
    msg_snd.segment_length = htons(sizeof(msg_snd) - sizeof(UINT16));
    msg_snd.prim_id = htonl(12000); //P_TT_CPLOGCTRL_ACCESS_REQ
    msg_snd.interface_version = 0x02;

	puts("Connecting..........");
	if(connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
	else
		puts("Connected..........");

	send(hSocket, (char *)&msg_snd, sizeof(msg_snd), 0);
	//recv(hSocket, opmsg, );

	
	printf("WANG!");
	getchar();
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

