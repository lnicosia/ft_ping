#include "libft.h"
#include "libmft.h"
#include "ip.h"
#include "options.h"
#include "send_probes.h"
#include "time_utils.h"
#include <netinet/ip.h>
#include <stdio.h>

/**	Print ping statistics before stopping the program
*/

void	print_statistics(void)
{
	printf("\n--- %s ping statistics ---\n", g_global_data.av);
	printf("%u packets transmitted, %u received,",
		g_global_data.packets_transmitted,
		g_global_data.packets_received);
	double	packet_ratio = (double)g_global_data.packets_received
		/ (double)g_global_data.packets_transmitted;
	if (g_global_data.errors > 0)
		printf(" +%ld errors,", g_global_data.errors);
	printf(" %.f%% packet loss, time %dms\n",
		100 - (100 * packet_ratio), 0);
	double avg = (double)g_global_data.time_sum
		/ (double)g_global_data.packets_received;
	double mdev = ft_sqrt((double)g_global_data.square_sum
		/ (double)g_global_data.packets_received - avg * avg);
	if (g_global_data.packets_received > 0)
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			(double)g_global_data.min_time / 1000.0,
			avg / 1000.0,
			(double)g_global_data.max_time / 1000.0, mdev / 1000.0);
	else
		printf("\n");
}

/** Set out packet data for every probe
*/

void	set_out_packet_data(struct icmp_packet* out_packet)
{
	ft_bzero(out_packet, sizeof(*out_packet));
	out_packet->header.type = ICMP_ECHO;
	out_packet->header.code = 0;
	out_packet->header.un.echo.id = 4242;
	unsigned int i;
	for (i = 0; i < sizeof(out_packet->payload) - 1; i++)
		out_packet->payload[i] = (char)(i + '0');
	out_packet->payload[i] = '\0';
	//	Update sequence (= received packets count) and checksum
	out_packet->header.un.echo.sequence = ++g_global_data.packets_transmitted;
	out_packet->header.checksum = checksum(out_packet, sizeof(*out_packet));
}

/**	Print received packet info: size, ip, etc
*/

void	print_received_packet_info(ssize_t received_bytes,
	struct timeval current_time, suseconds_t time_diff, struct msghdr *msghdr)
{
	struct ip *ip = (struct ip*)msghdr->msg_iov->iov_base;
	struct icmphdr *icmphdr = (struct icmphdr*)(msghdr->msg_iov->iov_base
		+ IP_HEADER_SIZE);
	char	buff[INET6_ADDRSTRLEN];
	void	*addr = &(ip->ip_src);
	inet_ntop(AF_INET, addr, buff, INET6_ADDRSTRLEN);
	if (g_global_data.opt & OPT_PRINT_TIMESTAMP)
	{
		printf("[%ld.%ld] ", current_time.tv_sec, current_time.tv_usec);
	}
	if (icmphdr->type != ICMP_ECHOREPLY)
	{
		print_response_error(received_bytes, ip, icmphdr);
		g_global_data.packets_received--;
		g_global_data.errors++;
	}
	else
	{
		printf("%ld bytes from %s: icmp_seq=%d ttl=%hhu time=%.2f ms\n",
			received_bytes - (ssize_t)IP_HEADER_SIZE,
			buff, g_global_data.packets_transmitted,
			ip->ip_ttl, (double)(time_diff) / 1000.0);
	}
}

void	send_and_receive_probe(int sckt,
 struct icmp_packet *out_packet, struct msghdr *msghdr)
{
	suseconds_t		send_time;
	suseconds_t		recv_time;
	suseconds_t		time_diff;
	struct timeval	recv_timeval;
	ssize_t			received_bytes;

	//	Set out packet data
	set_out_packet_data(out_packet);
	send_time = get_time();
	if (sendto(sckt, out_packet, sizeof(*out_packet), 0,
		(struct sockaddr*)&g_global_data.dst_ip.ip4,
		sizeof(g_global_data.dst_ip.ip4)) <= 0)
	{
		perror("ft_ping: sendto");
		close(sckt);
		free_and_exit_failure();
	}
	if (g_global_data.opt & OPT_VERBOSE)
	{
		printf("Sending\n");
		print_icmp_header(&out_packet->header);
	}
	received_bytes = recvmsg(sckt, msghdr, 0);
	if (received_bytes == -1)
	{
		if (g_global_data.opt & OPT_VERBOSE)
		{
			perror("\e[31mrecvmsg");
			dprintf(STDERR_FILENO, "\e[0m");
		}
		else if (g_global_data.opt & OPT_V)
		{
		}
	}
	else //	We received a message!
	{
		if (g_global_data.opt & OPT_VERBOSE)
		{
			printf("Receiving\n");
			print_ip4_header(msghdr->msg_iov->iov_base);
			print_icmp_header(msghdr->msg_iov->iov_base + IP_HEADER_SIZE);
		}
		g_global_data.packets_received++;
		//	Compare current time to when we sent the packet 
		if (gettimeofday(&recv_timeval, NULL) == -1)
			dprintf(STDERR_FILENO, "ft_ping: gettimeofday error\n");
		recv_time = recv_timeval.tv_sec * 1000000 + recv_timeval.tv_usec;
		time_diff = recv_time - send_time;
		print_received_packet_info(received_bytes, recv_timeval, time_diff,
			msghdr);
		//	Update min, max and average timers
		if (time_diff > g_global_data.max_time)
			g_global_data.max_time = time_diff;
		if (time_diff < g_global_data.min_time)
			g_global_data.min_time = time_diff;
		g_global_data.time_sum += time_diff;
		g_global_data.square_sum += time_diff * time_diff;
	}
}

/**	Send ICMP Echo out_packets to destination IP
*/

int	send_probes(int sckt)
{
	struct icmp_packet	out_packet;
	//struct icmp_packet	in_packet;	
	char				in_packet[IP_PACKET_SIZE];
	struct msghdr		msghdr;	
	struct iovec		iov;

	ft_bzero(&in_packet, sizeof(in_packet));
	ft_bzero(&msghdr, sizeof(msghdr));
	iov.iov_base = &in_packet;
	iov.iov_len = sizeof(in_packet);
	msghdr.msg_iov = &iov;
	msghdr.msg_iovlen = 1;

	//	Send out_packets while we can
	while (1)
	{
		//	Interrupt signal -> print stats, close socket and exit
		if (g_global_data.interrupt_flag == 1)
		{
			print_statistics();
			close(sckt);
			break ;
		}
		//	Alarm signal -> send a new echo and wait for an answer
		if (g_global_data.alarm_flag == 1)
		{
			g_global_data.alarm_flag = 0;
			send_and_receive_probe(sckt, &out_packet, &msghdr);
			alarm(1);
		}
	}
	return 0;
}
