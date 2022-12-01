#include "ft_ping.h"
#include "options.h"
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <netdb.h>

void	print_msg(int type, int code)
{
	switch (type)
	{
		case ICMP_ECHOREPLY:
			printf("Echo Reply\n");
			break;
		case ICMP_DEST_UNREACH:
			switch (code)
			{
				case ICMP_NET_UNREACH:
					printf("Destination Net Unreachable\n");
					break;
				case ICMP_HOST_UNREACH:
					printf("Destination Host Unreachable\n");
					break;
				case ICMP_PROT_UNREACH:
					printf("Destination Protocol Unreachable\n");
					break;
				case ICMP_PORT_UNREACH:
					printf("Destination Port Unreachable\n");
					break;
				case ICMP_FRAG_NEEDED:
					printf("Frag needed");
					break;
				case ICMP_SR_FAILED:
					printf("Source Route Failed\n");
					break;
				case ICMP_NET_UNKNOWN:
					printf("Destination Net Unknown\n");
					break;
				case ICMP_HOST_UNKNOWN:
					printf("Destination Host Unknown\n");
					break;
				case ICMP_HOST_ISOLATED:
					printf("Source Host Isolated\n");
					break;
				case ICMP_NET_ANO:
					printf("Destination Net Prohibited\n");
					break;
				case ICMP_HOST_ANO:
					printf("Destination Host Prohibited\n");
					break;
				case ICMP_NET_UNR_TOS:
					printf("Destination Net Unreachable for Type of Service\n");
					break;
				case ICMP_HOST_UNR_TOS:
					printf("Destination Host Unreachable for Type of Service\n");
					break;
				case ICMP_PKT_FILTERED:
					printf("Packet filtered\n");
					break;
				case ICMP_PREC_VIOLATION:
					printf("Precedence Violation\n");
					break;
				case ICMP_PREC_CUTOFF:
					printf("Precedence Cutoff\n");
					break;
			}
			break;
		case ICMP_SOURCE_QUENCH:
			printf("Source Quench\n");
			break;
		case ICMP_REDIRECT:
			switch (code)
			{
				case ICMP_REDIR_NET:
					printf("Redirect Network");
					break;
				case ICMP_REDIR_HOST:
					printf("Redirect Host");
					break;
				case ICMP_REDIR_NETTOS:
					printf("Redirect Type of Service and Network");
					break;
				case ICMP_REDIR_HOSTTOS:
					printf("Redirect Type of Service and Host");
					break;
			}
			break;
		case ICMP_ECHO:
			printf("Echo Request\n");
			break;
		case ICMP_TIME_EXCEEDED:
			switch(code)
			{
				case ICMP_EXC_TTL:
					printf("Time to live exceeded\n");
					break;
				case ICMP_EXC_FRAGTIME:
					printf("Frag reassembly time exceeded\n");
					break;
			}
			break;
		case ICMP_PARAMETERPROB:
			printf("Parameter problem\n");
			break;
		case ICMP_TIMESTAMP:
			printf("Timestamp\n");
			break;
		case ICMP_TIMESTAMPREPLY:
			printf("Timestamp Reply\n");
			break;
		case ICMP_INFO_REQUEST:
			printf("Information Request\n");
			break;
		case ICMP_INFO_REPLY:
			printf("Information Reply\n");
			break;
		case ICMP_MASKREQ:
			printf("Address Mask Request\n");
			break;
		case ICMP_MASKREPLY:
			printf("Address Mask Reply\n");
			break;
	}
}

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
		print_msg(icmphdr->type, icmphdr->code);
	}
}
