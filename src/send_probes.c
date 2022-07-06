#include "libft.h"
#include "ip.h"
#include "options.h"
#include "ft_ping.h"
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/time.h>

#define PACKET_SIZE 64

struct	icmp_packet
{
	struct	icmphdr	header;
	char			msg[PACKET_SIZE - sizeof(struct icmphdr)];
};

/**	Send ICMP Echo out_packets to destination IP
*/

int	send_probes(int sckt)
{
	struct icmp_packet	out_packet;
	struct msghdr	in_packet;	
	struct timeval	send_time;
	struct timeval	recv_time;

	//	Send the first out_packet

	//	Send out_packets while we can
	while (1)
	{
		if (g_global_data.interrupt_flag == 1)
		{
			printf("\n--- %s ping statistics ---\n", g_global_data.av);
			printf("%u out_packets transmitted, %u received, %u%% packet loss, "\
				"time %dms\n",
				g_global_data.packets_transmitted,
				g_global_data.packets_received,
				100 - (100 * (g_global_data.packets_received
					/ g_global_data.packets_transmitted)),
				0);
			printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
				(float)g_global_data.min_time / 1000.0f,
				(float)g_global_data.avg_time / 1000.0f,
				(float)g_global_data.max_time / 1000.0f,
				(float)g_global_data.mdev_time / 1000.0f);
			close(sckt);
			break ;
		}
		if (g_global_data.alarm_flag == 1)
		{
			g_global_data.alarm_flag = 0;
			ft_bzero(&out_packet, sizeof(out_packet));
			ft_bzero(&in_packet, sizeof(in_packet));
			out_packet.header.type = ICMP_ECHO;
			out_packet.header.un.echo.id = 4242;
			unsigned int i;
			for (i = 0; i < sizeof(out_packet.msg) - 1; i++)
				out_packet.msg[i] = (char)(i + '0');
			out_packet.msg[i] = '\0';
			out_packet.header.un.echo.sequence = ++g_global_data.packets_transmitted;
			out_packet.header.checksum = checksum(&out_packet, sizeof(out_packet));
			gettimeofday(&send_time, NULL);
			if (sendto(sckt, &out_packet, sizeof(out_packet), 0,
				(struct sockaddr*)&g_global_data.dst_ip.ip4,
				sizeof(g_global_data.dst_ip.ip4)) <= 0)
			{
				perror("ft_ping: sendto");
				close(sckt);
				break ;
			}
			if (recvmsg(sckt, &in_packet, 0) == 1)
			{
				dprintf(STDERR_FILENO, "No out_packet received\n");
				perror("");
			}
			else
			{
				//printf("Msg name = '%s'\n", (char*)in_packet.msg_name);
				//printf("Msg name len = %d\n", in_packet.msg_namelen);
				//printf("Msg control len = %ld\n", in_packet.msg_controllen);
				g_global_data.packets_received++;
				gettimeofday(&recv_time, NULL);
				suseconds_t	diff = recv_time.tv_usec - send_time.tv_usec;
				printf("%d bytes from %s: icmp_seq=%d",
					PACKET_SIZE, g_global_data.dst_ip.str4,
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
				g_global_data.avg_time =
					(g_global_data.avg_time + diff) / 2;
			}
			alarm(1);
		}
	}
	return 0;
}
