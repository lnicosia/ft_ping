#ifndef OPTIONS_H
# define OPTIONS_H

# define OPT_VERBOSE (1UL << 0)

int	parse_ping_options(int ac, char **av, unsigned long long *opt);

#endif
