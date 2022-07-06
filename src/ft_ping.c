#include "libft.h"
#include "options.h"
#include "send_probes.h"
#include "ft_ping.h"
#include "sighandlers.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>

t_global_data	init_global_data(void)
{
	t_global_data	res;

	ft_bzero(&res, sizeof(res));
	res.opt |= OPT_IP4;
	res.ttl = 64;
	res.alarm_flag = 1;
	res.min_time = 1000000;
	return res;
}

t_global_data	g_global_data;

int	init_socket(void)
{
	int	sckt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sckt == -1)
	{
		perror("ft_ping: socket");
		return -1;
	}
	if (setsockopt(sckt, SOL_IP, IP_TTL,
		&g_global_data.ttl, sizeof(g_global_data.ttl)) != 0)
	{
		perror("ft_ping: setsockopt");
		close(sckt);
		return -1;
	}
	if (setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO,
		&g_global_data.timeout, sizeof(g_global_data.timeout)) != 0)
	{
		perror("ft_ping: setsockopt");
		close(sckt);
		return -1;
	}
	return sckt;
}

int	ft_ping(int ac, char **av)
{
	g_global_data = init_global_data();
	int ret = parse_ping_options(ac, av);
	if (ret != 0)
	{
		if (ret == 3)
			return 0;
		return ret;
	}
	if (getuid() != 0)
		return dprintf(STDERR_FILENO, "Must be root to run the program\n");
	if (signal(SIGINT, &interrupt_sighandler) == SIG_ERR)
	{
		perror("signal");
		return -1;
	}
	if (signal(SIGALRM, &alarm_sighandler) == SIG_ERR)
	{
		perror("signal");
		return -1;
	}
	char	src_ip_name[256];
	ft_bzero(src_ip_name, sizeof(src_ip_name));
	if (ft_gethostname(src_ip_name, sizeof(src_ip_name)) == -1)
	{
		dprintf(STDERR_FILENO, "Unable to find system's hostname\n");
		return -1;
	}
	g_global_data.src_ip = resolve_hostname(src_ip_name);
	g_global_data.dst_ip = resolve_hostname(g_global_data.av);
	printf("PING %s (%s) %d(%d) bytes of data.\n",
	g_global_data.av, g_global_data.dst_ip.str4, 0, 0);
	int	sckt = init_socket();
	if (sckt == -1)
		return 2;
	send_probes(sckt);
	return 0;
}
