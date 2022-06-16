#include "libft.h"
#include <stdio.h>

int	main(int ac, char **av)
{
	if (ac < 1)
	{
		custom_error("ping: missing host operand\n");
		custom_error("Try 'ping --help' or 'ping --usage' for more information\n.");
		return 64;
	}
	return ft_ping(ac, av);
}