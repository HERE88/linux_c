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

#include <math.h>

#include <stdbool.h>

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
/*********************************************INS BELOW*********************************************/

#define LOG_INFO_E(x) LOG_PRINT x
#define LOG_PRINT(...) printf(__VA_ARGS__)

/*********************************************INS BELOW*********************************************/

typedef struct PC5LinkIdentifierST
{
    UINT32 source_l2_id;        // Source IPv4 addr
    UINT32 destination_l2_id;   // Destination IPv4 addr

    UINT32 service_id;      // V2X service types
    UINT16 aid;             // Application Layer ID

}PC5LinkIdentifier;

char srcId[3] = {0xC0, 0xA8, 0x01};


typedef struct PC5LinkNodeSt
{
    PC5LinkIdentifier uLink;
    struct PC5LinkNodeSt * next;

}PC5LinkNode;

int listSize = 0;
PC5LinkNode * head = NULL;

PC5LinkNode *initLinkList();


PC5LinkNode *initLinkList()
{
    PC5LinkNode *nodePtr = (PC5LinkNode *)malloc(sizeof(PC5LinkNode));
    if(NULL == nodePtr) {
        printf("initLinkList malloc failed\n");
    }
    else {
        nodePtr->next = NULL;
    }

    return nodePtr;
}

void insertLinkList(PC5LinkIdentifier *ip);
void deleteLinkList(PC5LinkIdentifier *ip);
bool findInLinkList(PC5LinkIdentifier *ip);
void printLinkList();

void deinitLinkList(); // WHT

void insertLinkList(PC5LinkIdentifier *ip)
{
    PC5LinkNode *nodePtr;

    if(findInLinkList(ip)) return;  // already exist

    nodePtr = (PC5LinkNode *)malloc(sizeof(PC5LinkNode));
    memcpy(&nodePtr->uLink, ip, sizeof(PC5LinkIdentifier));

    if(NULL == head->next)
    {
        head->next = nodePtr;
        nodePtr->next = NULL;
    }
    else
    {
        nodePtr->next = head->next;
        head->next = nodePtr;
    }
    listSize += 1;
    printf("insertLinkList success\n");
}

void deleteLinkList(PC5LinkIdentifier *ip)
{
    PC5LinkNode *tmp;
    PC5LinkNode *tp = head;
    while(NULL != tp->next)
    {
        if((tp->next->uLink.source_l2_id == ip->source_l2_id) &&
            (tp->next->uLink.destination_l2_id == ip->destination_l2_id) &&
            (tp->next->uLink.service_id == ip->service_id))
        {
            tmp = tp->next;
            tp->next = tp->next->next;
            free(tmp);

            listSize -= 1;
            printf("deleteLinkList success\n");
            break;
        }

        tp = tp->next;
    }
}

bool findInLinkList(PC5LinkIdentifier *ip)
{
    PC5LinkNode *tp = head;
    while(NULL != tp->next)
    {
        if((tp->next->uLink.source_l2_id == ip->source_l2_id) &&
            (tp->next->uLink.destination_l2_id == ip->destination_l2_id) &&
            (tp->next->uLink.service_id == ip->service_id))
        {
            return true;
        }

        tp = tp->next;
    }
    return false;
}

void printLinkList()
{
    PC5LinkNode *tp = head;
    while(NULL != tp->next)
    {
        printf("source_l2_id      =%d\n", tp->next->uLink.source_l2_id);
        printf("destination_l2_id =%d\n", tp->next->uLink.destination_l2_id);
        printf("service_id        =%d\n", tp->next->uLink.service_id);

        tp = tp->next;
    }
}

UINT32 findInLinkListEx(PC5LinkIdentifier *ip)
{
    PC5LinkNode *tp = head;
    while(NULL != tp->next)
    {
        if((tp->next->uLink.destination_l2_id == ip->destination_l2_id) &&
            (tp->next->uLink.service_id == ip->service_id))
        {
            return tp->next->uLink.source_l2_id;
        }

        tp = tp->next;
    }
    return -1;
}

void deinitLinkList() // WHT
{
    PC5LinkNode *tp = head;
    while(NULL != tp && NULL != tp->next)
    {
        deleteLinkList(&tp->next->uLink);
    }
}

int main(int argc, char *argv[])
{
    bool find;

    head = initLinkList(); // init

    PC5LinkIdentifier ip;

    printf("==========================\n");

    ip.source_l2_id = 111;
    ip.destination_l2_id = 112;
    ip.service_id = 113;
    insertLinkList(&ip);  // insert: 111,112,113
    printf("listSize=%d\n", listSize);
    printf("==========================\n");

    insertLinkList(&ip);  // again insert
    printf("listSize=%d\n", listSize);
    printf("==========================\n");

    ip.source_l2_id = 121;
    ip.destination_l2_id = 122;
    ip.service_id = 123;
    insertLinkList(&ip);  // insert: [121,122,123]
    printf("listSize=%d\n", listSize);
    printLinkList();  // print
    printf("==========================\n");

    ip.source_l2_id = 111;
    ip.destination_l2_id = 112;
    ip.service_id = 113;
    find = findInLinkList(&ip);  // find:  [111,112,113]
    printf("find=%d\n", (int)find);
    printf("==========================\n");


    ip.source_l2_id = 111;
    ip.destination_l2_id = 112;
    ip.service_id = 113;
    deleteLinkList(&ip);  // delete: [111,112,113]
    printf("listSize=%d\n", listSize);
    printLinkList();  // print
    printf("==========================\n");

    find = findInLinkList(&ip); // find: [111,112,113]
    printf("find=%d\n", (int)find);
    printf("==========================\n");

    ip.source_l2_id = 1233;
    ip.destination_l2_id = 122;
    ip.service_id = 123;
    printf("find src=%d\n", findInLinkListEx(&ip));   // findEx: [112,113]
    printf("==========================\n");

    ip.source_l2_id = 141;
    ip.destination_l2_id = 142;
    ip.service_id = 143;
    insertLinkList(&ip);  // insert: [141,142,143]
    printf("listSize=%d\n", listSize);
    printLinkList();  // print
    printf("==========================\n");

    deinitLinkList(); // deinit ALL
    printf("listSize=%d\n", listSize);
    printLinkList();  // print    
    printf("==========================\n");

    //sleep(0xffffffff);
    return 0;

}


