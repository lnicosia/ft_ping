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
	res.timeout.tv_sec = 1;
	res.timeout.tv_usec = 0;
	res.interval.tv_sec = 0;
	res.interval.tv_usec = 1000000;
	res.payload_size = 56;
	res.icmp_packet_size = res.payload_size + ICMP_HEADER_SIZE;
	res.ip_packet_size = res.icmp_packet_size + IP_HEADER_SIZE;
	return res;
}

t_global_data	g_global_data;

int	init_socket(void)
{
	g_global_data.sckt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (g_global_data.sckt == -1)
	{
		perror("ft_ping: socket");
		return -1;
	}
	if (setsockopt(g_global_data.sckt, SOL_IP, IP_TTL,
		&g_global_data.ttl, sizeof(g_global_data.ttl)) != 0)
	{
		perror("ft_ping: can't set unicast time-to-live");
		close(g_global_data.sckt);
		return -1;
	}
	if (setsockopt(g_global_data.sckt, SOL_SOCKET, SO_RCVTIMEO,
		&g_global_data.timeout, sizeof(g_global_data.timeout)) != 0)
	{
		perror("ft_ping: setsockopt");
		close(g_global_data.sckt);
		return -1;
	}
	return g_global_data.sckt;
}

int	ft_ping(int ac, char **av)
{
	g_global_data = init_global_data();
	int ret = parse_ping_options(ac, av);
	if (ret != 0)
	{
		//	Version print (-V)
		if (ret == PRINT_VERSION)
			return 0;
		//	Error
		return ret;
	}
	if (getuid() != 0)
	{
		dprintf(STDERR_FILENO, "Must be root to run the program\n");
		return 2;
	}
	if (signal(SIGINT, &interrupt_sighandler) == SIG_ERR
		|| signal(SIGALRM, &alarm_sighandler) == SIG_ERR
		|| signal(SIGQUIT, &quit_sighandler) == SIG_ERR)
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
	g_global_data.dst_ip = resolve_hostname(g_global_data.av);
	//	If it is a direct IP, do not try to resolve when receiving packets
	if (ft_strequ(g_global_data.dst_ip.str4, g_global_data.av)
		|| ft_strequ(g_global_data.av, "0"))
		g_global_data.direct_ip = 1;
	g_global_data.sckt = init_socket();
	if (g_global_data.sckt == -1)
		return 2;
	if (send_probes())
		free_and_exit_failure();
	free_and_exit_success();
	return 0;
}
