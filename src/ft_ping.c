#include "libft.h"
#include "ft_ping.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

int	ft_ping(int ac, char **av)
{
	unsigned long long	opt;
	
	int ret = parse_ping_options(ac, av, &opt);
	if (ret == -1)
		return -1;
	if (getuid() != 0)
		return custom_error("Must be root to run the program\n");
	resolve_hostname(av[1]);
	int	sckt = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sckt == -1)
		return ft_perror("socket");
	return 0;
}
