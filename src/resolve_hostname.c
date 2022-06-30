#include "libft.h"
#include "ip.h"
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

void		print_addrinfo(struct addrinfo *ai)
{
	if (ai == NULL)
		return ;
	printf("Flags = %d\n", ai->ai_flags);
	printf("Family = ");
	switch (ai->ai_family)
	{
		case AF_UNSPEC:
			printf("AF_UNSPEC\n");
			break;
		case AF_INET:
			printf("AF_INET\n");
			break;
		case AF_INET6:
			printf("AF_INET6\n");
			break;
		default:
			printf("???\n");
			break;
	}
	printf("Socket type = ");
	switch (ai->ai_socktype)
	{
		case SOCK_STREAM:
			printf("SOCK_STREAM\n");
			break;
		case SOCK_DGRAM:
			printf("SOCK_DGRAM\n");
			break;
		case SOCK_RAW:
			printf("SOCK_RAW\n");
			break;
		case SOCK_PACKET:
			printf("SOCK_PACKET\n");
			break;
		case SOCK_RDM:
			printf("SOCK_RDM\n");
			break;
	}
	printf("Protocol = %d\n", ai->ai_protocol);
}

t_ip		resolve_hostname(char *hostname)
{
	char	ipstr[INET6_ADDRSTRLEN];
	char	iptype;
	t_ip	res;

	struct addrinfo *ai;
	struct addrinfo hints;
	ft_bzero(&hints, sizeof(hints));
	ai = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_RAW;
	ft_bzero(&res, sizeof(res));
	if (getaddrinfo(hostname, NULL, &hints, &ai))
	{
		perror("getaddrinfo");
		return res;
	}
	struct addrinfo *tmp = ai;
	while (tmp)
	{
		//printf("\nAddrinfo:\n");
		//print_addrinfo(tmp);
		iptype = '?';
		void	*addr = NULL;
		if (tmp->ai_family == AF_INET)
		{
			struct sockaddr_in *ip4 = (struct sockaddr_in*)tmp->ai_addr;
			addr = &(ip4->sin_addr);
			ft_memcpy(&res.ip4, ip4, sizeof(res));
			iptype = '4';
		}
		else if (tmp->ai_family == AF_INET6)
		{
			struct sockaddr_in6 *ip6 = (struct sockaddr_in6*)tmp->ai_addr;
			addr = &(ip6->sin6_addr);
			ft_memcpy(&res.ip6, ip6, sizeof(res));
			iptype = '6';
		}
		inet_ntop(tmp->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
		printf("IPv%c = %s\n", iptype, ipstr);
		tmp = tmp->ai_next;
	}
	freeaddrinfo(ai);
	return res;
}
