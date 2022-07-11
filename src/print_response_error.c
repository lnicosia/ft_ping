#include "ft_ping.h"
#include <netinet/ip_icmp.h>
#include <stdio.h>

static const char *error_list[] =
{
	[ICMP_ECHOREPLY] = "ICMP_ECHOREPLY",
	[ICMP_ECHO] = "ICMP_ECHO",
	[ICMP_TIME_EXCEEDED] = "Time to live exceeded",
};

void	print_response_error(struct ip *ip, struct icmphdr *icmphdr)
{
	char	buff[INET6_ADDRSTRLEN];
	void	*addr = &(ip->ip_src);
	inet_ntop(AF_INET, addr, buff, INET6_ADDRSTRLEN);

	printf("From %s icmp_seq=%d ", buff, g_global_data.packets_transmitted);
	printf("%s\n", error_list[icmphdr->type]);
}
