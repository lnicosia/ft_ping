#include "options.h"
#include "libft.h"

int		print_version(void)
{
	ft_bprintf(0, "lnicosia's ft_ping version 1.0\n");
	ft_bprintf(0, "This program is free software; you may redistribute it\n");
	ft_bprintf(0, "This program has absolutely no warranty\n");
	return (2);
}

int		print_usage_stdin(void)
{
	ft_bprintf(0, "Usage: ft_ping [option(s)] [file(s)]\n");
	ft_bprintf(0, " List symbols in [file(s)] (a.out by default).\n");
	ft_bprintf(0, " The options are:\n");
	ft_bprintf(0, "  -h, --help\t\tprint help and exit\n");
	ft_bprintf(0, "  -v\t\t\terbose output\n");
	ft_bprintf(0, "  -V, --version\t\tprint version and exit\n");
	ft_bprintf(0, "Report bugs to <https://github.com/lnicosia/ft_nm>.\n");
	return (2);
}

int		print_usage(void)
{
	return (1);
}

int		check_opt(char *av, int *opt)
{
	if (*av == 'h')
		return print_usage_stdin();
	else if (*av == 'V')
		return print_version();
	else if (*av == 'v')
		*opt |= OPT_VERBOSE;
	else if (*av != 'e')
	{
		custom_error("ft_ping: invalid option -- '%s'\n", av);
		return print_usage();
	}
	return (0);
}

int		parse_option_line(char *av, int *opt)
{
	int	ret;

	if (ft_strbegin(av, "--"))
	{
		if (ft_strequ(av, "--verbose"))
			*opt |= OPT_VERBOSE;
		else if (ft_strnequ(av, "--help", ft_strlen(av)))
			return print_usage_stdin();
		else if (ft_strnequ(av, "--version", ft_strlen(av)))
			return print_version();
		else
		{
			custom_error("ft_ping: unrecognized option '%s'\n", av);
			return (print_usage());
		}
	}
	else
	{
		av++;
		while (*av)
		{
			if ((ret = check_opt(av, opt)) != 0)
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

int		parse_nm_options(int ac, char **av, int *opt, int* nb_files)
{
	int	i;
	int	ret;
	
	i = 1;
	while (i < ac)
	{
		if (is_arg_an_option_line(av[i]))
		{
			if ((ret = parse_option_line(av[i], opt)) != 0)
				return (ret);
		}
		else
			(*nb_files)++;
		i++;
	}
	return (0);
}
