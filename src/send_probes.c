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
	return 0;
}
