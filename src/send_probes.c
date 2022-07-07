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
	out_packet->header.un.echo.id = 4242;
	unsigned int i;
	for (i = 0; i < sizeof(out_packet->msg) - 1; i++)
		out_packet->msg[i] = (char)(i + '0');
	out_packet->msg[i] = '\0';
	//	Update sequence (= received packets count) and checksum
	out_packet->header.un.echo.sequence = ++g_global_data.packets_transmitted;
	out_packet->header.checksum = checksum(out_packet, sizeof(*out_packet));
}

/**	Print received packet info: size, ip, etc
*/

void	print_received_packet_info(ssize_t received_bytes,
	struct timeval current_time, suseconds_t time_diff)
{
	if (g_global_data.opt & OPT_PRINT_TIMESTAMP)
	{
		printf("[%ld.%ld] ", current_time.tv_sec, current_time.tv_usec);
	}
	printf("%ld bytes from %s: icmp_seq=%d",
		received_bytes, g_global_data.dst_ip.str4,
		g_global_data.packets_transmitted);
	if ((double)time_diff / 1000.0 > (double)g_global_data.ttl)
	{
		printf(" Time to live exceeded\n");
		g_global_data.packets_received--;
		g_global_data.errors++;
	}
	else
	{
		printf(" ttl=%ld time=%.2f ms\n",
			g_global_data.ttl - 1, (double)(time_diff) / 1000.0);
	}
}

void	send_and_receive_probe(int sckt,
 struct icmp_packet *out_packet, struct msghdr *in_packet)
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
	received_bytes = recvmsg(sckt, in_packet, 0);
	if (received_bytes == -1)
	{
		if (g_global_data.opt & OPT_VERBOSE)
		{
			perror("recvmsg");
		}
		else if (g_global_data.opt & OPT_V)
		{
		}
	}
	else //	We received a message!
	{
		g_global_data.packets_received++;
		//	Compare current time to when we sent the packet 
		if (gettimeofday(&recv_timeval, NULL) == -1)
			dprintf(STDERR_FILENO, "ft_ping: gettimeofday error\n");
		recv_time = recv_timeval.tv_sec * 1000000 + recv_timeval.tv_usec;
		time_diff = recv_time - send_time;
		print_received_packet_info(received_bytes, recv_timeval, time_diff);
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
	struct msghdr	in_packet;	
	char			iov_buff[ICMP_PACKET_SIZE];
	struct iovec	iov;

	ft_bzero(&in_packet, sizeof(in_packet));
	iov.iov_base = iov_buff;
	iov.iov_len = sizeof(iov_buff);
	in_packet.msg_iov = &iov;
	in_packet.msg_iovlen = 1;

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
			send_and_receive_probe(sckt, &out_packet, &in_packet);
			alarm(1);
		}
	}
	return 0;
}
