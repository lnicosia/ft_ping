#include "libft.h"
#include "ip.h"
#include "options.h"
#include "ft_ping.h"
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/time.h>

struct	icmp_packet
{
	struct	icmphdr	header;
	char			msg[PACKET_SIZE - sizeof(struct icmphdr)];
};

/**	Print ping statistics before stopping the program
*/

void	print_statistics(void)
{
	printf("\n--- %s ping statistics ---\n", g_global_data.av);
	printf("%u packets transmitted, %u received, %u%% packet loss, "\
		"time %dms\n",
		g_global_data.packets_transmitted,
		g_global_data.packets_received,
		100 - (100 * (g_global_data.packets_received
			/ g_global_data.packets_transmitted)),
		0);
	if (g_global_data.packets_received > 0)
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			(float)g_global_data.min_time / 1000.0f,
			(float)g_global_data.avg_time / 1000.0f,
			(float)g_global_data.max_time / 1000.0f,
			(float)g_global_data.mdev_time / 1000.0f);
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

/**	Send ICMP Echo out_packets to destination IP
*/

int	send_probes(int sckt)
{
	struct icmp_packet	out_packet;
	struct msghdr	in_packet;	
	struct timeval	send_time;
	struct timeval	recv_time;
	ssize_t			received_bytes;
	char			iov_buff[PACKET_SIZE];
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
			//	Set out packet data
			set_out_packet_data(&out_packet);
			gettimeofday(&send_time, NULL);
			if (sendto(sckt, &out_packet, sizeof(out_packet), 0,
				(struct sockaddr*)&g_global_data.dst_ip.ip4,
				sizeof(g_global_data.dst_ip.ip4)) <= 0)
			{
				perror("ft_ping: sendto");
				close(sckt);
				break ;
			}
			if ((received_bytes = recvmsg(sckt, &in_packet, MSG_DONTWAIT)) == -1)
			{
			}
			else
			{
				g_global_data.packets_received++;
				gettimeofday(&recv_time, NULL);
				suseconds_t	diff = recv_time.tv_usec - send_time.tv_usec;
				printf("%ld bytes from %s: icmp_seq=%d",
					received_bytes, g_global_data.dst_ip.str4,
					g_global_data.packets_received);
				if ((double)diff / 1000.0 > (double)g_global_data.ttl)
					printf(" Time to live exceeded\n");
				else
					printf(" ttl=%ld time=%.2f ms\n",
						g_global_data.ttl - 1, (float)(diff) / 1000.0f);
				if (diff > g_global_data.max_time)
					g_global_data.max_time = diff;
				if (diff < g_global_data.min_time)
					g_global_data.min_time = diff;
				g_global_data.avg_time += diff;
			}
			alarm(1);
		}
	}
	return 0;
}
