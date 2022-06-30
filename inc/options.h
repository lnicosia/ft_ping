#ifndef OPTIONS_H
# define OPTIONS_H

# define OPT_VERBOSE	(1UL << 0)
# define OPT_IP4		(1UL << 1)
# define OPT_IP6		(1UL << 2)

int	parse_ping_options(int ac, char **av, unsigned long long *opt);

#endif
