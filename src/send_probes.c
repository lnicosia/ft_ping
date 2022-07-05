#include "libft.h"
#include "ip.h"
#include "options.h"
#include "ft_ping.h"
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>

/**	Send ICMP Echo packets to destination IP
*/

int	send_probes(int sckt)
{
	char	out_buffer[400];
	char	in_buffer[400];
	struct ip *ip_buffer = (struct ip*)out_buffer;
	struct icmp *icmp_buffer = (struct icmp*)(ip_buffer + 1);
	(void)sckt;
		
	ft_bzero(out_buffer, sizeof(out_buffer));
	ft_bzero(in_buffer, sizeof(in_buffer));
	if (g_global_data.opt & OPT_IP4)
	{
		ip_buffer->ip_src = g_global_data.src_ip.ip4.sin_addr;
		ip_buffer->ip_dst = g_global_data.dst_ip.ip4.sin_addr;
		ip_buffer->ip_v = 4;
	}
	else if (g_global_data.opt & OPT_IP6)
	{
		printf("IPV6 not handled yet\n");
		return 0;
	}
	icmp_buffer->icmp_type = ICMP_ECHO;
	icmp_buffer->icmp_code = 0;
	icmp_buffer->icmp_id = 123;
	icmp_buffer->icmp_seq = 0;

	//	Send the first packet
	printf("%d bytes from %s: icmp_seq=%d ttl=%ld time=%.3f ms\n",
	0, g_global_data.dst_ip.str4, 0, g_global_data.ttl - 1, 0.0f);
	alarm(1);

	//	Send packets while we can
	while (1)
	{
		if (g_global_data.interrupt_flag == 1)
		{
			printf("\n--- %s ping statistics ---\n", g_global_data.av);
			printf("%ld packets transmitted, %ld received, %ld%% packet loss, "\
				"time %dms\n",
				g_global_data.packets_transmitted,
				g_global_data.packets_received,
				100 - (100 * (g_global_data.packets_received
					/ g_global_data.packets_transmitted)),
				0);
			printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
				0.0f, 0.0f, 0.0f, 0.0f);
			break ;
		}
		if (g_global_data.alarm_flag == 1)
		{
			g_global_data.alarm_flag = 0;
			g_global_data.packets_received++;
			g_global_data.packets_transmitted++;
			printf("%d bytes from %s: icmp_seq=%d ttl=%ld time=%.3f ms\n",
				0, g_global_data.dst_ip.str4, 0, g_global_data.ttl - 1, 0.0f);
			alarm(1);
		}
	}
	return 0;
}
