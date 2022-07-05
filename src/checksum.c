unsigned short	checksum(void *ptr, int len)
{
	unsigned short *buf = (unsigned short*)ptr;
	unsigned int	sum = 0;
	unsigned short	res = 0;
	
	for ( ; len > 1 ; len -= 2)
	{
		sum += *buf;
		buf++;
	}
	if (len == 1)
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	res = (unsigned short)~sum;
	return res;
}
