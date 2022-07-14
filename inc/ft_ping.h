#ifndef FT_PING_H
#define FT_PING_H

#include "ip.h"
#include <signal.h>

#ifndef EXTERN
# define EXTERN extern
#endif

#define PAYLOAD_SIZE		(size_t)56
#define ICMP_HEADER_SIZE	sizeof(struct icmphdr)
#define IP_HEADER_SIZE		sizeof(struct iphdr)
#define ICMP_PACKET_SIZE	ICMP_HEADER_SIZE + PAYLOAD_SIZE
#define IP_PACKET_SIZE		IP_HEADER_SIZE + ICMP_PACKET_SIZE

#define EWMA_ALPHA			0.125

typedef struct				s_global_data
{
	unsigned long long		opt;
	char					*av;
	t_ip					src_ip;
	t_ip					dst_ip;
	size_t					ttl;
	size_t					packet_size;
	size_t					errors;
	struct timeval			timeout;
	struct timeval			interval;
	suseconds_t				start_time;
	suseconds_t				last_probe;
	suseconds_t				min_time;
	suseconds_t				max_time;
	suseconds_t				time_sum;
	suseconds_t				square_sum;
	double					ewma;
	uint16_t				packets_transmitted;
	uint16_t				packets_received;
	int						custom_interval;
	int						direct_ip;
	unsigned int			count;
	volatile sig_atomic_t	interrupt_flag;
	volatile sig_atomic_t	alarm_flag;
	volatile sig_atomic_t	quit_flag;
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
