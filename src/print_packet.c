#include "send_probes.h"
#include "libft.h"
#include <stdio.h>

void	print_icmp_packet(struct icmp_packet *packet)
{
	printf("\e[36m+--------------+-----ICMP-----+-----------------+\n");
	//	Type
	printf("\e[36m|\e[33m    Type %-3hhu  \e[36m|\e[33m", packet->header.type);
	//	Code
	printf("    Code %-3hhu  \e[36m|\e[33m", packet->header.code);
	//	Checksum
	printf("  Checksum %-5hx \e[36m|\n", packet->header.checksum);
	printf("\e[36m+--------------+------+-------+-----------------+\n");
	//	Identifier
	printf("\e[36m|\e[33m       ID %-5hu      \e[36m|\e[33m", packet->header.un.echo.id);
	//	Sequence
	printf("       Sequence %-5hu    \e[36m|\n", packet->header.un.echo.sequence);
	printf("\e[36m+---------------------+-------------------------+\n");
	printf("\e[36m|                   \e[33mPayload                     \e[36m|\n");
	printf("\e[36m+-----------------------------------------------+\n");
	printf("\e[0m");
}
