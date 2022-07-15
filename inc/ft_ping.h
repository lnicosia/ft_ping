#ifndef FT_PING_H
#define FT_PING_H

#include "ip.h"
#include <netinet/ip_icmp.h>
#include <signal.h>

#ifndef EXTERN
# define EXTERN extern
#endif

#define ICMP_HEADER_SIZE	sizeof(struct icmphdr)
#define IP_HEADER_SIZE		sizeof(struct iphdr)

#define EWMA_ALPHA			0.125

/**	ICMP packet struct: a header + a message
*/

struct						icmp_packet
{
	struct icmphdr			header;
	char					*payload;
};

typedef struct				s_global_data
{
	unsigned long long		opt;
	char					*av;
	t_ip					src_ip;
	t_ip					dst_ip;
	size_t					ttl;
	size_t					errors;
	size_t					payload_size;
	size_t					ip_packet_size;
	size_t					icmp_packet_size;
	struct timeval			timeout;
	struct timeval			interval;
	struct icmp_packet		out_packet;
	char					*in_packet;
	char					*canonname;
	suseconds_t				start_time;
	suseconds_t				last_probe;
	suseconds_t				min_time;
	suseconds_t				max_time;
	suseconds_t				time_sum;
	suseconds_t				square_sum;
	double					ewma;
	int						sckt;
	int						custom_interval;
	int						direct_ip;
	unsigned int			count;
	volatile sig_atomic_t	interrupt_flag;
	volatile sig_atomic_t	alarm_flag;
	volatile sig_atomic_t	quit_flag;
	uint16_t				packets_transmitted;
	uint16_t				packets_received;
	uint16_t				id;
	char					padding[6];
}							t_global_data;

EXTERN t_global_data		g_global_data;

int							ft_ping(int ac, char **av);
unsigned short				checksum(void *ptr, size_t len);
int							ft_gethostname(char *name, size_t size);
int							print_usage_stderr(void);
void						free_and_exit_success(void);
void						free_and_exit_failure(void);

#endif
