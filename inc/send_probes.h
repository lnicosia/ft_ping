#ifndef SEND_PROBES_H
#define SEND_PROBES_H

#include "ft_ping.h"

int					send_probes(void);
void				print_icmp_header(struct icmphdr *header);
void				print_ip4_header(struct ip *header);
void				print_response_error(ssize_t received_bytes,
	struct ip *ip, struct icmphdr *icmphdr);

#endif
