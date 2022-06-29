#ifndef IP_H
#define IP_H

#include <arpa/inet.h>

union			u_ip_addr
{
	struct sockaddr_in	ip4;
	struct sockaddr_in6	ip6;

};

typedef	struct			s_ip
{
		union u_ip_addr	ip;
		uint16_t		ai_family;
		char			padding[2];
}						t_ip;

t_ip					resolve_hostname(char *hostname);

#endif
