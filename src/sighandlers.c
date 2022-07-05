#include "ft_ping.h"
#include <stdio.h>
#include <stdlib.h>

void	interrupt_sighandler(int value)
{
	(void)value;
	g_global_data.interrupt_flag = 1;
}

void	alarm_sighandler(int value)
{
	(void)value;
	g_global_data.alarm_flag = 1;
}
