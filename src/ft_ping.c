#include "libft.h"
#include "options.h"
#include "send_probes.h"
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
	t_ip				ip;
	
	int ret = parse_ping_options(ac, av, &opt);
	if (ret == -1)
		return -1;
	if (getuid() != 0)
		return custom_error("Must be root to run the program\n");
	ip = resolve_hostname(av[1]);
	if (ip.ai_family == AF_UNSPEC)
		return EXIT_FAILURE;
	int	sckt = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sckt == -1)
		return ft_perror("socket");
	send_probes(sckt, ip);
	return 0;
}
