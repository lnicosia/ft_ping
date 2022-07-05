#include "libft.h"
#include <stdio.h>
#include <fcntl.h>

/**	Retrieve system's hostname by reading /proc/sys/kernel/hostname contents
*/

int	ft_gethostname(char *name, size_t size)
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
