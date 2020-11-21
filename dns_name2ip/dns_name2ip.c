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

///////////////////////////////////////////////////
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define oops(msg) { perror(msg); exit(1);}
#include <ctype.h>

#include <unistd.h>

void fillip(char* buffer, const char* ip);
char* name2ip(const char* name);
/**
 *  * head len: 12
 *   * query: ? + 4
 *    * total: strlen(query.buffer) + 16
 *     */
typedef struct{
        u_short txid;
        u_short flag;
        u_short question;
        u_short answer;
        u_short authority;
        u_short additional;
        char buffer[256];
        struct {
                u_short type;
                u_short cls;
        }query;
}dns_req;
/** length of resp: 12 */
typedef struct{
        u_short txid;
        u_short flag;
        u_short question;
        u_short answer;
        u_short authority;
        u_short addtional;
        char buffer[1024];
        struct{
                u_short name;
                u_short type;
                u_short cls;
                u_short live_l;
                u_short live_h;
                u_short len;
                struct in_addr addr;
        }resp;
}dns_res;

// int hgetaddrinfo(const char *node, const char *service,
//                 const struct addrinfo *hints,
//                 struct addrinfo **res){
//         /*    int rc = getaddrinfo(node,service,hints,res);
//          *            if(rc == 0)//todo: != -> ==
//          *                            return rc;
//          *                                            */
//         char *ip = name2ip(node);                //
//         return getaddrinfo(ip,service,hints,res);   //
// }

char *get_dns_name()
{
    int fd;
    static char buf[1024],*pstr,*pdns;

    fd = open("resolv.conf",O_RDWR);  // "/etc/resolv.conf"
    read(fd,buf,1020);
    pstr = strstr(buf,"nameserver");
    pstr += strlen("nameserver");
    while(!isdigit(*pstr)){
        pstr ++;
    }
    pdns = pstr;

    while(isdigit(*pstr) || (*pstr == '.') ){
        pstr ++;
    }
    *pstr = '\0';
    close(fd);
    return pdns;
}


char * name2ip(const char *node){
        struct sockaddr_in dns;
        dns_req req;
        dns_res res;
        int sockid, len, index;
        //get_dns_name();
        char  dnsip[32];

        strcpy(dnsip,get_dns_name());

        sockid = socket(PF_INET, SOCK_DGRAM, 0);
        if(sockid == -1)
                oops("socket");
        memset((void*)&dns, 0, sizeof(dns));
        dns.sin_family = AF_INET;
        dns.sin_port = htons(53);
        dns.sin_addr.s_addr = inet_addr(dnsip);

        memset((void*)&req, 0, sizeof(req));
        req.txid = htons(0x4419);
        req.flag = htons(0x0100);
        req.question = htons(1);
        fillip(req.buffer,node);
        req.query.type=htons(1);
        req.query.cls=htons(1);
        memcpy(req.buffer+strlen(req.buffer)+1, (void*)(&req.query), sizeof(req.query));

        sendto(sockid, (void*)&req, strlen(req.buffer)+17, 0,
                        (struct sockaddr*)&dns, sizeof(dns));
        recvfrom(sockid, (void*)&res, sizeof(res), 0,
                        (struct sockaddr*)&dns, &len);
        index = strlen(res.buffer)+5;
        while(1){
                memcpy((void*)&(res.resp), res.buffer+index, 12);
                if(ntohs(res.resp.type)==1){
                        memcpy((void*)&(res.resp.addr), res.buffer+index+12, 4);
                        break;
                }
                index += ntohs(res.resp.len) + 12;
        }

        return inet_ntoa(res.resp.addr);
}

void fillip(char* buffer, const char* ip){
        int i,j=0;
        for(i = 0; ip[i] != 0; i ++){
                if(ip[i] != '.'){
                        buffer[i+1] = ip[i];
                }
                else{
                        buffer[j] = i - j;
                        j = i + 1;
                }
        }
        buffer[j] = i - j;
}

/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char * argv[])
{
    char *domain_name = "baidu.com";
    printf("Domain_name:[%s], IP:[%s]\n", domain_name, name2ip(domain_name));
    return 0;

}