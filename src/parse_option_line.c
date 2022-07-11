#include "options.h"
#include "libft.h"
#include "ft_ping.h"
#include <stdio.h>

int		print_version(void)
{
	printf("lnicosia's ft_ping version 1.0\n");
	printf("This program is free software; you may redistribute it\n");
	printf("This program has absolutely no warranty\n");
	return 3;
}

int		print_usage_stdin(void)
{
	printf("Usage: ft_ping [-DhvV] [-t ttl] destination\n");
	return 2;
}

int		print_usage_stderr(void)
{
	dprintf(STDERR_FILENO, "Usage: ft_ping [-DhvV] [-t ttl] destination\n");
	return 2;
}

int		check_opt(char *c, char **av, int i)
{
	if (*c == 'h')
		return print_usage_stderr();
	else if (*c == 'V')
		return print_version();
	else if (*c == 'v')
		g_global_data.opt |= OPT_V;
	else if (*c == '4')
		g_global_data.opt |= OPT_IP4;
	else if (*c == 'D')
		g_global_data.opt |= OPT_PRINT_TIMESTAMP;
	else if (*c == 't')
	{
		if (av[i + 1])
		{
			g_global_data.ttl = (size_t)ft_atoi(av[i + 1]);
			if (g_global_data.ttl == 0)
			{
				dprintf(STDERR_FILENO, "ft_ping: can't set unicast time-to-live:" \
					" Invalid argument\n");
				return 2;
			}
			else if (g_global_data.ttl > 255)
			{
				dprintf(STDERR_FILENO, "ft_ping: ttl %ld out of range\n",
					g_global_data.ttl);
				return 2;
			}
		}
	}
	else if (*c != 'e')
	{
		dprintf(STDERR_FILENO, "ft_ping: invalid option -- '%s'\n", c);
		return print_usage_stderr();
	}
	return (0);
}

int		parse_option_line(char *av, char **full_av, int i)
{
	int	ret;

	if (ft_strbegin(av, "--"))
	{
		if (ft_strequ(av, "--verbose"))
			g_global_data.opt |= OPT_VERBOSE;
		else if (ft_strnequ(av, "--help", ft_strlen(av)))
			return print_usage_stdin();
		else if (ft_strnequ(av, "--version", ft_strlen(av)))
			return print_version();
		else
		{
			dprintf(STDERR_FILENO, "ft_ping: invalid option '%s'\n", av);
			return print_usage_stderr();
		}
	}
	else
	{
		av++;
		while (*av)
		{
			if ((ret = check_opt(av, full_av, i)) != 0)
				return (ret);
			av++;
		}
	}
	return (0);
}

/*
**	Checks if the given string is an option line (starting with '-')
*/

int		is_arg_an_option_line(char *av)
{
	return (ft_strlen(av) > 1 && av[0] == '-');
}

/*
**	Parse all the options by checking arguments starting with '-'
*/

int		parse_ping_options(int ac, char **av)
{
	int	i;
	int	ret;
	
	i = 1;
	while (i < ac)
	{
		if (is_arg_an_option_line(av[i]))
		{
			if ((ret = parse_option_line(av[i], av, i)) != 0)
				return (ret);
		}
		else if (!(i - 1 > 0 && ft_strequ(av[i - 1], "-t")))
		{
			if (g_global_data.av != 0)
				g_global_data.opt |= OPT_MULTIPLE_ADDR;
			g_global_data.av = av[i];
		}
		i++;
	}
	if (g_global_data.av == NULL)
		return print_usage_stderr();
	return (0);
}
