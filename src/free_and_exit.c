#include "ft_ping.h"
#include "libft.h"
#include <stdlib.h>

void	free_all(void)
{
	if (g_global_data.out_packet.payload != NULL)
		ft_memdel((void**)&g_global_data.out_packet.payload);
	if (g_global_data.in_packet != NULL)
		ft_memdel((void**)&g_global_data.in_packet);
	if (g_global_data.canonname != NULL)
		ft_strdel(&g_global_data.canonname);
	close(g_global_data.sckt);
}

void	free_and_exit_success(void)
{
	free_all();
	exit(EXIT_SUCCESS);
}

void	free_and_exit_failure(void)
{
	free_all();
	exit(EXIT_FAILURE);
}
