#include "options.h"
#include "libft.h"
#include "libmft.h"
#include "ft_ping.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>

int		print_version(void)
{
	printf("lnicosia's ft_ping version 1.0\n");
	printf("This program is free software; you may redistribute it\n");
	printf("This program has absolutely no warranty\n");
	return PRINT_VERSION;
}

int		print_usage_stdin(void)
{
	printf("Usage: ft_ping [-DhvV] [-c count] [-i interval] [-s packetsize]");
	printf(" [-t ttl] [--verbose] destination\n");
	return OPTION_ERROR;
}

int		print_usage_stderr(void)
{
	dprintf(STDERR_FILENO, "Usage: ft_ping [-DhvV] [-c count] [-i interval] ");
	dprintf(STDERR_FILENO, " [-s packetsize] [-t ttl] [--verbose] destination\n");
	return OPTION_ERROR;
}

/*
**	Parse all the options by checking arguments starting with '-'
*/

int		parse_ping_options(int ac, char **av)
{
	int			opt, option_index = 0;
	char		*optarg = NULL;
	const char	*optstring = "DhvVt:4i:c:s:q";
	static struct option long_options[] =
	{
		{"help",	no_argument,		0, 'h'},
		{"version",	no_argument,		0, 'V'},
		{"verbose",	no_argument,		0,	0 },
		{"quiet",	no_argument,		0, 'q'},
		{"ttl",		required_argument,	0, 't'},
		{"interval",required_argument,	0, 'i'},
		{"count",	required_argument,	0, 'c'},
		{"size",	required_argument,	0, 's'},
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
			case 'q':
				g_global_data.opt |= OPT_QUIET;
				break;
			case '4':
				g_global_data.opt |= OPT_IP4;
				break;
			case 's':
			{
				long long packetsize = ft_atoll(optarg);
				if (packetsize < 0)
				{
					dprintf(STDERR_FILENO, "%s: illegal negative packet size %lld\n",
						av[0], packetsize);
					return OPTION_ERROR;
				}
				//if (packetsize > 65507)
				if (packetsize > 3336)
				{
					dprintf(STDERR_FILENO, "%s: packet size %lld is too large.",
						av[0], packetsize);
					dprintf(STDERR_FILENO, " Maximum is 65507\n");
					return OPTION_ERROR;
				}
				g_global_data.payload_size = (size_t)packetsize;
				g_global_data.icmp_packet_size = g_global_data.payload_size
					+ ICMP_HEADER_SIZE;
				g_global_data.ip_packet_size = g_global_data.icmp_packet_size
					+ IP_HEADER_SIZE;
				break;
			}
			case 't':
			{
				int	ttl = ft_atoi(optarg);
				if (ttl < 0 || ttl > 255)
				{
					dprintf(STDERR_FILENO, "%s: ttl %d out of range\n",
						av[0], ttl);
					return OPTION_ERROR;
				}
				g_global_data.ttl = (size_t)ttl;
				break;
			}
			case 'c':
			{
				long long count = ft_atoll(optarg);
				if (count <= 0 || count > INT_MAX)
				{
					dprintf(STDERR_FILENO, "%s: bad number of packets" \
						"to transmit.\n", av[0]);
					return OPTION_ERROR;
				}
				g_global_data.count = (unsigned int)count;
				break;
			}
			case 'i':
			{
				double	interval = ft_atof(optarg);
				if (interval < 0 || interval > 2147482)
				{
					dprintf(STDERR_FILENO, "%s: bad timing interval\n",
						av[0]);
					return OPTION_ERROR;
				}
				double	integral;
				double	fractional = modf(interval, &integral);
				g_global_data.interval.tv_sec = (time_t)integral;
				g_global_data.custom_interval = 1;
				g_global_data.interval.tv_usec =
					(suseconds_t)(fractional * 1000000);
				if (interval < 10e-7)
					g_global_data.interval.tv_usec = 1;
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
