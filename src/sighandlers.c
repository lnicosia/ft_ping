#include <stdio.h>
#include <stdlib.h>

void	interrupt_sighandler(int value)
{
	(void)value;
	printf("--- xx:xx:xx:xx ping statistics ---\n");
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
	0, 0, 100, 0);
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
	0.0f, 0.0f, 0.0f, 0.0f);
	exit(EXIT_SUCCESS);
}

void	alarm_sighandler(int value)
{
	(void)value;
	printf("%d bytes from xx:xx:xx:xx: icmp_seq=%d ttl=%d time=%.3f ms\n",
	0, 0, 0, 0.0f);
}
