

#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern int LTE_V2X_ParamInit();
extern int LTE_V2X_Tx_SendData(char* p_pSendData, int nDataLen);
extern int LTE_V2X_Rx_SetDevParam(void (*pFunRecvData)(char *p_cRecvBuf, int *pnRecvBufSize));



void recv(char * buf, int *len)
{
    char rd_buf[65536];
    
    snprintf(rd_buf, sizeof(rd_buf), "read:%s, len:%d", buf,  *len);
    printf("[%s]\n", rd_buf);
}

int main()
{
    int ret;
    
    ret = LTE_V2X_ParamInit();
    printf("ret=%d\n", ret);
    LTE_V2X_Rx_SetDevParam(recv);

while(1)
{
    LTE_V2X_Tx_SendData("WHTHAITAO", strlen("WHTHAITAO"));
    printf("222\n");
    usleep(1000*1000);
}


    usleep(0xffffffff);
    return 0;

}