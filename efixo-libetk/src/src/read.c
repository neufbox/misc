
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <etk/log.h>

ssize_t safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t full_read(int fd, void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;
	while (len > 0) {
		cc = safe_read(fd, buf, len);
		if (cc < 0)
			return cc;	/* read() returns -1 on failure. */
		if (cc == 0)
			break;
		buf = ((char *)buf) + cc;
		total += cc;
		len -= cc;
	}

	return total;
}

ssize_t file_open_read_close(char const *file, void *buf, size_t len)
{
	int fd;
	int count;

	dbg("(%s, %p, %zu)", file, buf, len);

	if ((fd = open(file, O_RDONLY)) < 0) {
		return -1;
	}

	count = full_read(fd, buf, len);

	close(fd);

	return count;
}
