#include "send_probes.h"
#include "libft.h"
#include <stdio.h>

void	print_ip4_header(struct ip *header)
{
	printf("\e[32m+----------------------+----IP-----+------------------+\n");
	//	Version
	printf("\e[32m|\e[33m  Version %-2hhu \e[32m/\e[33m", header->ip_v);
	//	IHL
	printf(" IHL %-2hhu \e[32m|\e[33m", header->ip_hl);
	//	Type of service
	printf("   TOS %-3hx \e[32m|\e[33m", header->ip_tos);
	//	Total length
	printf("  Total len %-5hd \e[32m|\n", header->ip_len);

	printf("\e[32m+----------------------+-+---------+------------------+\n");

	//	Identification
	printf("\e[32m|\e[33m         ID %-5hu       \e[32m|\e[33m", header->ip_id);
	//	Flags / Offset
	printf("         Offset %-5hu       \e[32m|\n", header->ip_off);

	printf("\e[32m+--------------+---------+--------+-------------------+\n");

	//	TTL
	printf("\e[32m|\e[33m    TTL %-3hhu   \e[32m|\e[33m", header->ip_ttl);
	//	Protocol
	printf("    Protocol %-3hhu  \e[32m|\e[33m", header->ip_p);
	//	Header cheskum
	printf("   Checksum %-5hx  \e[32m|\n", header->ip_sum);

	printf("\e[32m+--------------+------------------+-------------------+\n");

	char	buff[INET6_ADDRSTRLEN];
	void	*addr = &header->ip_src;
	inet_ntop(AF_INET, addr, buff, INET6_ADDRSTRLEN); 
	printf("\e[32m|\e[33m                  Source addr %-12s           \e[32m|\n", buff);

	printf("\e[32m+-----------------------------------------------------+\n");

	addr = &header->ip_dst;
	inet_ntop(AF_INET, addr, buff, INET6_ADDRSTRLEN); 
	printf("\e[32m|\e[33m                  Dest addr %-12s             \e[32m|\n", buff);

	printf("\e[32m+-----------------------------------------------------+\n");
}

void	print_icmp_header(struct icmphdr *header)
{
	printf("\e[36m+--------------+-----ICMP-----+-----------------+\n");

	//	Type
	printf("\e[36m|\e[33m    Type %-3hhu  \e[36m|\e[33m", header->type);
	//	Code
	printf("    Code %-3hhu  \e[36m|\e[33m", header->code);
	//	Checksum
	printf("  Checksum %-5hx \e[36m|\n", header->checksum);

	printf("\e[36m+--------------+------+-------+-----------------+\n");

	//	Identifier
	printf("\e[36m|\e[33m       ID %-5hu      \e[36m|\e[33m", header->un.echo.id);
	//	Sequence
	printf("       Sequence %-5hu    \e[36m|\n", header->un.echo.sequence);

	printf("\e[36m+---------------------+-------------------------+\n");

	printf("\e[36m|                  \e[33mPayload                      \e[36m|\n");

	printf("\e[36m+-----------------------------------------------+\n");
	printf("\e[0m");
}
