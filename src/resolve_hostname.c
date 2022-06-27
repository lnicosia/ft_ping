#include "libft.h"
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

int	resolve_hostname(char *hostname)
{
	char				ipstr[INET6_ADDRSTRLEN];
	char				iptype;
	
	struct addrinfo *ai;
	struct addrinfo hints;
	ft_bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_RAW;
	if (getaddrinfo(hostname, NULL, &hints, &ai))
		return ft_perror("getaddrinfo");
	struct addrinfo *tmp = ai;
	while (tmp)
	{
		iptype = '?';
		void	*addr = NULL;
		if (tmp->ai_family == AF_INET)
		{
			struct sockaddr *ip4 = (struct sockaddr*)tmp->ai_addr;
			addr = &(ip4->sa_data);
			iptype = '4';
		}
		else if (tmp->ai_family == AF_INET6)
		{
			struct sockaddr *ip6 = (struct sockaddr*)tmp->ai_addr;
			addr = &(ip6->sa_data);
			iptype = '6';
		}
		inet_ntop(tmp->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
		printf("IPv%c = %s\n", iptype, ipstr);
		tmp = tmp->ai_next;
	}
	freeaddrinfo(ai);
	return 0;
}
