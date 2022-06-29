#include "libft.h"
#include "ip.h"
#include <netinet/ip.h>

int	send_probes(int sckt, t_ip ip)
{
		char	buffer[400];
		struct ip *ip_buffer = (struct ip*)buffer;
		struct icmp *icmp_buffer = (struct icmp*)(ip_buffer + 1);
		(void)sckt;
		(void)icmp_buffer;
		(void)ip;

		ft_bzero(buffer, sizeof(buffer));

		return 0;
}
