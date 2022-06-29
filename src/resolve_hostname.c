#include "libft.h"
#include "ip.h"
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

t_ip		resolve_hostname(char *hostname)
{
	char	ipstr[INET6_ADDRSTRLEN];
	char	iptype;
	t_ip	res;
	
	struct addrinfo *ai;
	struct addrinfo hints;
	ft_bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_RAW;
	ft_bzero(&res, sizeof(res));
	res.ai_family = AF_UNSPEC;
	if (getaddrinfo(hostname, NULL, &hints, &ai))
	{
		perror("getaddrinfo");
		return res;
	}
	struct addrinfo *tmp = ai;
	while (tmp)
	{
		iptype = '?';
		void	*addr = NULL;
		if (tmp->ai_family == AF_INET)
		{
			struct sockaddr_in *ip4 = (struct sockaddr_in*)tmp->ai_addr;
			addr = &(ip4->sin_addr);
			ft_memcpy(&res.ip.ip4, ip4, sizeof(res));
			iptype = '4';
		}
		else if (tmp->ai_family == AF_INET6)
		{
			struct sockaddr_in6 *ip6 = (struct sockaddr_in6*)tmp->ai_addr;
			addr = &(ip6->sin6_addr);
			ft_memcpy(&res.ip.ip6, ip6, sizeof(res));
			iptype = '6';
		}
		inet_ntop(tmp->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
		printf("IPv%c = %s\n", iptype, ipstr);
		tmp = tmp->ai_next;
	}
	freeaddrinfo(ai);
	return res;
}
