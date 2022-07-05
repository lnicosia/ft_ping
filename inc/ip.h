#ifndef IP_H
#define IP_H

#include <arpa/inet.h>

typedef	struct	s_ip
{
	struct sockaddr_in	ip4;
	struct sockaddr_in6	ip6;
	char				str4[INET_ADDRSTRLEN];
	char				str6[INET6_ADDRSTRLEN];
	char				padding[2];
}				t_ip;

t_ip			resolve_hostname(char *hostname);

#endif
