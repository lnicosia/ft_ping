#include "options.h"
#include "libft.h"
#include "ft_ping.h"
#include <stdio.h>

int		print_version(void)
{
	printf("lnicosia's ft_ping version 1.0\n");
	printf("This program is free software; you may redistribute it\n");
	printf("This program has absolutely no warranty\n");
	return (2);
}

int		print_usage_stdin(void)
{
	printf("Usage: ft_ping [option(s)] [file(s)]\n");
	printf(" List symbols in [file(s)] (a.out by default).\n");
	printf(" The options are:\n");
	printf("  -h, --help\t\tprint help and exit\n");
	printf("  -v\t\t\terbose output\n");
	printf("  -V, --version\t\tprint version and exit\n");
	return (2);
}

int		print_usage(void)
{
	dprintf(STDERR_FILENO, "Usage: ft_ping [option(s)] [file(s)]\n");
	dprintf(STDERR_FILENO, " List symbols in [file(s)] (a.out by default).\n");
	dprintf(STDERR_FILENO, " The options are:\n");
	dprintf(STDERR_FILENO, "  -h, --help\t\tprint help and exit\n");
	dprintf(STDERR_FILENO, "  -v\t\t\terbose output\n");
	dprintf(STDERR_FILENO, "  -V, --version\t\tprint version and exit\n");
	return (1);
}

int		check_opt(char *av)
{
	if (*av == 'h')
		return print_usage_stdin();
	else if (*av == 'V')
		return print_version();
	else if (*av == 'v')
		g_global_data.opt |= OPT_VERBOSE;
	else if (*av != 'e')
	{
		dprintf(STDERR_FILENO, "ft_ping: invalid option -- '%s'\n", av);
		return print_usage();
	}
	return (0);
}

int		parse_option_line(char *av)
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
			return print_usage();
		}
	}
	else
	{
		av++;
		while (*av)
		{
			if ((ret = check_opt(av)) != 0)
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
			if ((ret = parse_option_line(av[i])) != 0)
				return (ret);
		}
		g_global_data.av = av[i];
		i++;
	}
	if (g_global_data.av == NULL)
		return print_usage();
	return (0);
}
