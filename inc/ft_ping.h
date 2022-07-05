#ifndef FT_PING_H
#define FT_PING_H

#include "ip.h"
#include <signal.h>

#ifndef EXTERN
# define EXTERN extern
#endif

typedef struct				s_global_data
{
	volatile sig_atomic_t	interrupt_flag;
	volatile sig_atomic_t	alarm_flag;
	unsigned long long		opt;
	char					*av;
	t_ip					src_ip;
	t_ip					dst_ip;
}							t_global_data;

EXTERN t_global_data		g_global_data;

int							ft_ping(int ac, char **av);
unsigned short				checksum(void *ptr, size_t len);
int							ft_gethostname(char *name, size_t size);
int							print_usage(void);
void						free_and_exit_success(void);
void						free_and_exit_failure(void);

#endif
