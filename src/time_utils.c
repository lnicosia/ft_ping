#include "ft_ping.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

suseconds_t		get_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
	{
		perror("ft_ping: gettimeofday");
		free_and_exit_failure();
	}
	return (time.tv_sec * 1000000 + time.tv_usec);
}
