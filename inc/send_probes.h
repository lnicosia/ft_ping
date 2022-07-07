#ifndef SEND_PROBES_H
#define SEND_PROBES_H

#include "ft_ping.h"
#include <netinet/ip_icmp.h>

/**	ICMP packet struct: a header + a message
*/

struct	icmp_packet
{
	struct	icmphdr	header;
	char			msg[PAYLOAD_SIZE];
};

int		send_probes(int sckt);

#endif
