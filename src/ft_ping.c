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
	return res;
}

t_global_data	g_global_data;

int	ft_ping(int ac, char **av)
{
	t_ip				ip;
	
	g_global_data = init_global_data();
	int ret = parse_ping_options(ac, av);
	if (ret == -1)
		return -1;
	if (getuid() != 0)
		return custom_error("Must be root to run the program\n");
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
	ip = resolve_hostname(av[1]);
	printf("PING %s (%s)\n", av[1], "?");
	int	sckt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sckt == -1)
		return ft_perror("socket");
	printf("%d bytes from xx:xx:xx:xx: icmp_seq=%d ttl=%d time=%.3f ms\n",
	0, 0, 0, 0.0f);
	alarm(1);
	while (1)
	{
		if (g_global_data.interrupt_flag == 1)
		{
			printf("--- xx:xx:xx:xx ping statistics ---\n");
			printf("%d packets transmitted, %d received, %d%% packet loss, \
			time %dms\n", 0, 0, 100, 0);
			printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			0.0f, 0.0f, 0.0f, 0.0f);
			break ;
		}
		if (g_global_data.alarm_flag == 1)
		{
			g_global_data.alarm_flag = 0;
			printf("%d bytes from xx:xx:xx:xx: icmp_seq=%d ttl=%d time=%.3f ms\n",
			0, 0, 0, 0.0f);
			alarm(1);
		}
	}
	send_probes(sckt, ip);
	return 0;
}
