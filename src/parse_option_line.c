#include "options.h"
#include "libft.h"
#include "ft_ping.h"
#include <stdio.h>

int		print_version(void)
{
	printf("lnicosia's ft_ping version 1.0\n");
	printf("This program is free software; you may redistribute it\n");
	printf("This program has absolutely no warranty\n");
	return PRINT_VERSION;
}

int		print_usage_stdin(void)
{
	printf("Usage: ft_ping [-DhvV] [-t ttl] destination\n");
	return OPTION_ERROR;
}

int		print_usage_stderr(void)
{
	dprintf(STDERR_FILENO, "Usage: ft_ping [-DhvV] [-t ttl] destination\n");
	return OPTION_ERROR;
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
				return OPTION_ERROR;
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
**	Parse all the options by checking arguments starting with '-'
*/

int		parse_ping_options(int ac, char **av)
{
	int			opt, option_index = 0;
	char		*optarg = NULL;
	const char	*optstring = "DhvVt:4";
	static struct option long_options[] =
	{
		{"help",	no_argument,		0, 'h'},
		{"version",	no_argument,		0, 'V'},
		{"verbose",	no_argument,		0,	0 },
		{"ttl",		required_argument,	0, 't'},
		{0,			0,					0,	0 }
	};

	while ((opt = ft_getopt_long(ac, av, optstring, &optarg,
		long_options, &option_index)) != -1)
	{
		switch (opt)
		{
			//	Specific long options that have no short equivalent
			case 0:
			{
				if (ft_strequ(long_options[option_index].name, "verbose"))
					g_global_data.opt |= OPT_VERBOSE;
				break;
			}
			case 'v':
				g_global_data.opt |= OPT_V;
				break;
			case 'D':
				g_global_data.opt |= OPT_PRINT_TIMESTAMP;
				break;
			case '4':
				g_global_data.opt |= OPT_IP4;
				break;
			case 't':
			{
				int	ttl = ft_atoi(optarg);
				if (ttl < 0 || ttl > 255)
				{
					ft_dprintf(STDERR_FILENO, "%s: ttl %d out of range\n",
						av[0], ttl);
					return OPTION_ERROR;
				}
				g_global_data.ttl = (size_t)ttl;
				break;
			}
			case 'h':
				return print_usage_stderr();
			case 'V':
				return print_version();
			case '?':
				return print_usage_stderr();
		}
	}
	//	Retrieve the last given IP address
	for (int i = 1; i < ac; i++)
	{
		if (!is_arg_an_opt(av, i, optstring, long_options))
		{
			if (g_global_data.av != 0)
				g_global_data.opt |= OPT_MULTIPLE_ADDR;
			g_global_data.av = av[i];
		}
	}
	return 0;
}
