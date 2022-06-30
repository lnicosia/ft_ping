#ifndef IP_H
#define IP_H

#include <arpa/inet.h>

typedef	struct	s_ip
{
	struct sockaddr_in	ip4;
	struct sockaddr_in6	ip6;
}				t_ip;

t_ip			resolve_hostname(char *hostname);

#endif
