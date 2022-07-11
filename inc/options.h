#ifndef OPTIONS_H
# define OPTIONS_H

# define OPT_V					(1UL << 0)
# define OPT_IP4				(1UL << 1)
# define OPT_IP6				(1UL << 2)
# define OPT_PRINT_TIMESTAMP	(1UL << 3)
# define OPT_VERBOSE			(1UL << 4)
# define OPT_MULTIPLE_ADDR		(1UL << 5)

int	parse_ping_options(int ac, char **av);

#endif
