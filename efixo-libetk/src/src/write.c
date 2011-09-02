
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <etk/log.h>

ssize_t safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do {
		n = write(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;
	while (len > 0) {
		cc = safe_write(fd, buf, len);
		if (cc < 0)
			return cc;	/* write() returns -1 on failure. */
		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}
	return total;
}

ssize_t file_open_write_close(char const *file, void const *buf, size_t len)
{
	int fd;
	int count;

	dbg("(%s, %p, %zu)", file, buf, len);

	if ((fd = open(file, O_CREAT | O_WRONLY,
		       S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH |
		       S_IWOTH)) < 0) {
		return -1;
	}

	count = full_write(fd, buf, len);

	close(fd);

	return count;
}
