#ifndef SEND_PROBES_H
#define SEND_PROBES_H

#include "ft_ping.h"
#include <netinet/ip_icmp.h>

/**	ICMP packet struct: a header + a message
*/

struct				icmp_packet
{
	struct	icmphdr	header;
	char			payload[PAYLOAD_SIZE];
};

int					send_probes(int sckt);
void				print_icmp_header(struct icmphdr *header);
void				print_ip4_header(struct ip *header);
void				print_response_error(struct ip *ip, struct icmphdr *icmphdr);

#endif
