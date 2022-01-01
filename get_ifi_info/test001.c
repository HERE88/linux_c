#include	"unpifi.h"

int
main(int argc, char **argv)
{
	struct ifi_info	*ifi, *ifihead;
	struct sockaddr	*sa;
	u_char			*ptr;
	int				i, family;

	if (argc != 2)
	{
		printf("usage: prifinfo <inet4|inet6>\n");
        exit(0);
	}

	if (strcmp(argv[1], "inet4") == 0)
		family = AF_INET;
//#ifdef	IPv6
	else if (strcmp(argv[1], "inet6") == 0)
		family = AF_INET6;
//#endif
	else
	{
		printf("invalid <address-family>\n");
        exit(0);
	}

	for (ifihead = ifi = get_ifi_info(family); ifi != NULL; ifi = ifi->ifi_next) 
    {
		printf("%s: ", ifi->ifi_name);
		printf("<");
/* *INDENT-OFF* */
		if (ifi->ifi_flags & IFF_UP)			printf("UP ");
		if (ifi->ifi_flags & IFF_BROADCAST)		printf("BCAST ");
		if (ifi->ifi_flags & IFF_MULTICAST)		printf("MCAST ");
		if (ifi->ifi_flags & IFF_LOOPBACK)		printf("LOOP ");
		if (ifi->ifi_flags & IFF_POINTOPOINT)	printf("P2P ");

		if (ifi->ifi_flags & IFF_RUNNING)	printf("RUN ");  // [WANGHAITAO]
		printf(">\n");
/* *INDENT-ON* */

		if (ifi->ifi_mtu != 0)
			printf("  MTU: %d\n", ifi->ifi_mtu);

		if ( (sa = ifi->ifi_addr) != NULL)
			printf("  IP addr: %s\n", sock_ntop_host(sa, sizeof(*sa)));
		if ( (sa = ifi->ifi_brdaddr) != NULL)
			printf("  broadcast addr: %s\n", sock_ntop_host(sa, sizeof(*sa)));
		if ( (sa = ifi->ifi_dstaddr) != NULL)
			printf("  destination addr: %s\n", sock_ntop_host(sa, sizeof(*sa)));

        if ( (sa = ifi->ifi_hwaddr) != NULL)   // [WANGHAITAO]
        {
            unsigned char mac[6];
            memcpy(mac, ifi->ifi_hwaddr->sa_data, 6);
            printf("  hardware addr: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
                        
	}
	free_ifi_info(ifihead);
	exit(0);
}
