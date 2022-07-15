#include "ft_ping.h"
#include "options.h"
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <netdb.h>

static const char *error_list[] =
{
	[ICMP_ECHOREPLY] = "ICMP_ECHOREPLY",
	[ICMP_ECHO] = "ICMP_ECHO",
	[ICMP_TIME_EXCEEDED] = "Time to live exceeded",
};

void	print_response_error(ssize_t received_bytes,
	struct ip *ip, struct icmphdr *icmphdr)
{
	char	buff[INET6_ADDRSTRLEN];
	void	*addr = &(ip->ip_src);
	inet_ntop(AF_INET, addr, buff, INET6_ADDRSTRLEN);

	char	host[512];
	struct sockaddr_in in_addr;
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = 0;
	in_addr.sin_addr = ip->ip_src;
	struct sockaddr *final_addr = (struct sockaddr*)&in_addr;
	if (getnameinfo(final_addr, sizeof(in_addr), host, sizeof(host),
		NULL, 0, 0))
	{
		perror("ft_ping: getnameinfo");
	}
	if (g_global_data.opt & OPT_V)
	{
		printf("%ld bytes from %s: type = %d code = %d\n",
			received_bytes - (ssize_t)IP_HEADER_SIZE,
			buff, icmphdr->type, icmphdr->code);
	}
	else
	{
		if (g_global_data.direct_ip == 1 || g_global_data.opt & OPT_NUMERIC_ONLY)
			printf("From %s", buff);
		else
			printf("From %s (%s)", host, buff);
		printf(" icmp_seq=%d ", g_global_data.packets_transmitted);
		printf("%s\n", error_list[icmphdr->type]);
	}
}
