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
#include <netdb.h>
#include  <sys/socket.h>

/* include sock_ntop */
char *
sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char		portstr[8];
    static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		if (ntohs(sin->sin_port) != 0) {
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return(str);
	}
/* end sock_ntop */

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		str[0] = '[';
		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
			return(NULL);
		if (ntohs(sin6->sin6_port) != 0) {
			snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return(str);
		}
		return (str + 1);
	}
#endif

#ifdef	AF_UNIX
	case AF_UNIX: {
		struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return(str);
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK: {
		struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

		if (sdl->sdl_nlen > 0)
			snprintf(str, sizeof(str), "%*s (index %d)",
					 sdl->sdl_nlen, &sdl->sdl_data[0], sdl->sdl_index);
		else
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
		return(str);
	}
#endif
	default:
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}
    return (NULL);
}

char *
Sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char	*ptr;

	if ( (ptr = sock_ntop(sa, salen)) == NULL)
		printf("sock_ntop error\n");	/* inet_ntop() sets errno */
	return(ptr);
}

int udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	/* Get a list of potential server addresses */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE|AI_NUMERICSERV;
	hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

	if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", host, serv, gai_strerror(n));
        return -2;
	}
	ressave = res;

	/* Walk the list for one that we can successfully connect to */
	do{
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(sockfd < 0)
			continue; /* Socket failed, try the next */

		if(bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break; /* Success */

		if(close(sockfd) < 0)
		{ /* Connect failed, try another */
            fprintf(stderr, "open_clientfd: close failed: %s\n", strerror(errno));
            return -1;
        }
	}while((res = res->ai_next) != NULL);

	if(res == NULL)
	{
		printf("udp_server error for %s, %s\n", host, serv);
		return -1; /* All connects failed */
	}

	if(addrlenp)
		*addrlenp = res->ai_addrlen; /*return size of protocol address*/

	freeaddrinfo(ressave);

	return sockfd;
}

int main()
{

	int sockfd;
	ssize_t n;
	char buff[1024];
	time_t ticks;
	socklen_t len;
	struct sockaddr_storage cliaddr;
	sockfd = udp_server(NULL, "9080", NULL);

	len = sizeof(cliaddr);
	n = recvfrom(sockfd, buff, 1024, 0, (struct sockaddr *)&cliaddr, &len);
	printf("datagram from %s\n", Sock_ntop((struct sockaddr *)&cliaddr, len));

	for(;;)
	{
		// len = sizeof(cliaddr);
		// n = recvform(sockfd, buff, 1024, (struct sockaddr *)&cliaddr, &len);
		// printf("datagram from %s\n", Sock_ntop((struct sockaddr *)&cliaddr, len));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\n", ctime(&ticks));
		sendto(sockfd, buff ,strlen(buff), 0, (struct sockaddr *)&cliaddr, len);
		printf("send:%s",buff);
		sleep(2);
	}

	return 0;
}