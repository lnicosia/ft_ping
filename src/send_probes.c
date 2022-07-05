#include "libft.h"
#include "ip.h"
#include "options.h"
#include "ft_ping.h"
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <fcntl.h>

/**
**	Retrieve system's hostname by reading /proc/sys/kernel/hostname contents
*/

static int	ft_gethostname(char *name, size_t size)
{
	char	buffer[256];
	int		fd;
	ssize_t	ret;

	if ((fd = open("/proc/sys/kernel/hostname", O_RDONLY)) == -1)
	{
		perror("open");
		return -1;
	}
	if ((ret = read(fd, buffer, 255)) == -1)
	{
		perror("read");
		close(fd);
		return -1;
	}
	buffer[ret] = 0;
	//	Do not copy more than size bytes
	size_t	line_len = ft_strlen(buffer);
	size_t	copy_length = line_len < size ? line_len : size;
	ft_memcpy(name, buffer, copy_length);
	//	Remove the \n at the end
	if (name[copy_length - 1] == '\n')
		name[copy_length - 1] = 0;
	close(fd);
	return 0;
}

int	send_probes(int sckt, t_ip ip)
{
	char	out_buffer[400];
	char	in_buffer[400];
	struct ip *ip_buffer = (struct ip*)out_buffer;
	struct icmp *icmp_buffer = (struct icmp*)(ip_buffer + 1);
	(void)sckt;
		
	ft_bzero(out_buffer, sizeof(out_buffer));
	ft_bzero(in_buffer, sizeof(in_buffer));
	char src_name[256];
	if (ft_gethostname(src_name, sizeof(src_name)) == -1)
	{
		custom_error("Unable to find system's hostname\n");
		return -1;
	}
	printf("Src IP:\n");
	printf("Name = '%s'\n", src_name);
	t_ip	src_ip = resolve_hostname(src_name);
	if (g_global_data.opt & OPT_IP4)
	{
		ip_buffer->ip_src = src_ip.ip4.sin_addr;
		ip_buffer->ip_dst = ip.ip4.sin_addr;
		ip_buffer->ip_v = 4;
	}
	else if (g_global_data.opt & OPT_IP6)
	{
		printf("IPV6 not handled yet\n");
		return 0;
	}

	icmp_buffer->icmp_type = ICMP_ECHO;
	icmp_buffer->icmp_code = 0;
	icmp_buffer->icmp_id = 123;
	icmp_buffer->icmp_seq = 0;
	return 0;
}
