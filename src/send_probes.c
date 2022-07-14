#include "libft.h"
#include "libmft.h"
#include "ip.h"
#include "options.h"
#include "send_probes.h"
#include "time_utils.h"
#include <netinet/ip.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>

/**	Print ping statistics after a SIGQUIT
*/

void	print_statistics(void)
{
	double	packet_ratio = (double)g_global_data.packets_received
		/ (double)g_global_data.packets_transmitted;
	printf("%d/%d packets, %.f%% loss", g_global_data.packets_received,
		g_global_data.packets_transmitted, 100 - (100 * packet_ratio));
	if (g_global_data.packets_received == 0)
	{
		printf("\n");
		return ;
	}
	double avg = (double)g_global_data.time_sum
		/ (double)g_global_data.packets_received;
	printf(", min/avg/ewma/max = %.3f/%.3f/%.3f/%.3f ms\n",
			(double)g_global_data.min_time / 1000.0,
			avg / 1000.0, g_global_data.ewma,
			(double)g_global_data.max_time / 1000.0);
}

/**	Print ping statistics before stopping the program
*/

void	print_final_statistics(void)
{
	printf("\n--- %s ping statistics ---\n", g_global_data.av);
	printf("%u packets transmitted, %u received,",
		g_global_data.packets_transmitted,
		g_global_data.packets_received);
	double	packet_ratio = (double)g_global_data.packets_received
		/ (double)g_global_data.packets_transmitted;
	if (g_global_data.errors > 0)
		printf(" +%ld errors,", g_global_data.errors);
	suseconds_t	diff = g_global_data.last_probe - g_global_data.start_time;
	printf(" %.f%% packet loss, time %ldms\n",
		100 - (100 * packet_ratio), diff / 1000);
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
	ft_bzero(&out_packet->header, ICMP_HEADER_SIZE);
	ft_bzero(out_packet->payload, g_global_data.payload_size);
	out_packet->header.type = ICMP_ECHO;
	out_packet->header.code = 0;
	out_packet->header.un.echo.id = 4242;
	unsigned int i;
	for (i = 0; i < g_global_data.payload_size - 1; i++)
		out_packet->payload[i] = (char)(i + '0');
	out_packet->payload[i] = '\0';
	//	Update sequence (= received packets count) and checksum
	out_packet->header.un.echo.sequence = ++g_global_data.packets_transmitted;
	out_packet->header.checksum = checksum(out_packet,
		g_global_data.icmp_packet_size);
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
	if (icmphdr->type != ICMP_ECHOREPLY && icmphdr->type != ICMP_ECHO)
	{
		print_response_error(received_bytes, ip, icmphdr);
		g_global_data.packets_received--;
		g_global_data.errors++;
	}
	else
	{
		char	host[512];
		char	serv[512];
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = 0;
		addr.sin_addr = ip->ip_src;
		struct sockaddr *final_addr = (struct sockaddr*)&addr;
		if (getnameinfo(final_addr, sizeof(addr), host, sizeof(host),
			serv, sizeof(serv), 0))
		{
			perror("ft_ping: getnameinfo");
		}
		printf("%ld bytes from ", received_bytes - (ssize_t)IP_HEADER_SIZE);
		if (g_global_data.direct_ip == 1)
			printf("%s", buff);
		else
			printf("%s (%s)", host, buff);
		printf(": icmp_seq=%d ttl=%hhu time=%.2f ms\n",
			g_global_data.packets_transmitted,
			ip->ip_ttl, (double)(time_diff) / 1000.0);

	}
}

void	send_and_receive_probe(struct icmp_packet *out_packet, struct msghdr *msghdr)
{
	suseconds_t		send_time;
	suseconds_t		recv_time;
	suseconds_t		time_diff;
	struct timeval	recv_timeval;
	ssize_t			received_bytes;

	//	Set out packet data
	if (g_global_data.custom_interval == 0)
		alarm(1);
	set_out_packet_data(out_packet);
	send_time = get_time();
	if (g_global_data.opt & OPT_VERBOSE)
	{
		printf("Sending\n");
		print_icmp_header(&out_packet->header);
	}
	if (sendto(g_global_data.sckt, out_packet, g_global_data.icmp_packet_size,
		0, (struct sockaddr*)&g_global_data.dst_ip.ip4,
		sizeof(g_global_data.dst_ip.ip4)) <= 0)
	{
		perror("sendto");
		free_and_exit_failure();
	}
	if (g_global_data.packets_transmitted == 1)
		printf("PING %s (%s) %ld(%ld) bytes of data.\n",
		g_global_data.av, g_global_data.dst_ip.str4, g_global_data.payload_size, 
			g_global_data.opt & OPT_MULTIPLE_ADDR ?
				g_global_data.ip_packet_size + 40 : g_global_data.ip_packet_size);
	g_global_data.last_probe = get_time();
	//	If recvmsg was interrupted by a too fast SIGALARM, relaunch it
	while ((received_bytes = recvmsg(g_global_data.sckt, msghdr, 0)) == -1
		&& errno == EINTR)
		continue;
	if (received_bytes == -1)
	{
		if (g_global_data.opt & OPT_VERBOSE)
		{
			perror("\e[31mrecvmsg");
			dprintf(STDERR_FILENO, "\e[0m");
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
		//	Get the time manually because option -D needs it
		if (gettimeofday(&recv_timeval, NULL) == -1)
			perror("ft_ping: gettimeofday");
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
		if (g_global_data.packets_received == 1)
			g_global_data.ewma = (double)time_diff / 1000.0;
		else
			g_global_data.ewma = EWMA_ALPHA * (double)time_diff / 1000.0
				+ (1.0 - EWMA_ALPHA) * g_global_data.ewma;
	}
}

/**	Send ICMP Echo out_packets to destination IP
*/

int	send_probes(void)
{
	struct msghdr		msghdr;	
	struct iovec		iov;

	g_global_data.out_packet.payload =
		(char*)malloc(g_global_data.payload_size);
	if (g_global_data.out_packet.payload == NULL)
		free_and_exit_failure();
	g_global_data.in_packet =
		(char*)malloc(g_global_data.ip_packet_size);
	if (g_global_data.in_packet == NULL)
		free_and_exit_failure();
	ft_bzero(g_global_data.in_packet, g_global_data.ip_packet_size);
	ft_bzero(&msghdr, sizeof(msghdr));
	iov.iov_base = g_global_data.in_packet;
	iov.iov_len = g_global_data.ip_packet_size;
	msghdr.msg_iov = &iov;
	msghdr.msg_iovlen = 1;
	g_global_data.start_time = get_time();

	//	If -i was given, use this instead of alarm() to
	//	set timers < 1sec
	if (g_global_data.custom_interval == 1)
	{
		struct itimerval timer;
		timer.it_interval = g_global_data.interval;
		timer.it_value = g_global_data.interval;
		if (setitimer(ITIMER_REAL, &timer, NULL))
		{
			perror("ping: setitimer");
			return 2;
		}
	}

	unsigned int count = 0;
	//	Send out_packets while we can
	while (g_global_data.count == 0 ||
		count < g_global_data.count)
	{
		//	Interrupt signal -> print stats, close socket and exit
		if (g_global_data.interrupt_flag == 1)
		{
			break ;
		}
		//	Quit signal -> print stats
		if (g_global_data.quit_flag == 1)
		{
			g_global_data.quit_flag = 0;
			print_statistics();
		}
		//	Alarm signal -> send a new echo and wait for an answer
		if (g_global_data.alarm_flag == 1)
		{
			g_global_data.alarm_flag = 0;
			send_and_receive_probe(&g_global_data.out_packet, &msghdr);
			count++;
		}
	}
	print_final_statistics();
	return 0;
}
