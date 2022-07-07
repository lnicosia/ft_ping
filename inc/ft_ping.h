#ifndef FT_PING_H
#define FT_PING_H

#include "ip.h"
#include <signal.h>

#ifndef EXTERN
# define EXTERN extern
#endif

#define PAYLOAD_SIZE		(size_t)56
#define ICMP_HEADER_SIZE	sizeof(struct icmphdr)
#define IC_HEADER_SIZE		sizeof(struct ichdr)
#define ICMP_PACKET_SIZE	PAYLOAD_SIZE + ICMP_HEADER_SIZE
#define IP_PACKET_SIZE		ICMP_PACKET_SIZE + ICMP_PACKET_SIZE

typedef struct				s_global_data
{
	volatile sig_atomic_t	interrupt_flag;
	volatile sig_atomic_t	alarm_flag;
	unsigned long long		opt;
	char					*av;
	t_ip					src_ip;
	t_ip					dst_ip;
	size_t					ttl;
	size_t					packet_size;
	size_t					errors;
	struct timeval			timeout;
	suseconds_t				min_time;
	suseconds_t				max_time;
	suseconds_t				time_sum;
	suseconds_t				square_sum;
	uint16_t				packets_transmitted;
	uint16_t				packets_received;
	char					padding[4];
}							t_global_data;

EXTERN t_global_data		g_global_data;

int							ft_ping(int ac, char **av);
unsigned short				checksum(void *ptr, size_t len);
int							ft_gethostname(char *name, size_t size);
int							print_usage_stderr(void);
void						free_and_exit_success(void);
void						free_and_exit_failure(void);

#endif
