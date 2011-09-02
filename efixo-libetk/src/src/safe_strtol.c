
#include <stdlib.h>
#include <errno.h>

#include <etk/log.h>

long safe_strtol(char const *buf, long def)
{
	long ret;

	if (*buf == '\0') {
		info("%s - buffer is empty", __func__);
		return def;
	}
	errno = 0;
	ret = strtol(buf, NULL, 10);
	if (errno != 0) {
		err("%s - conversion failed, buffer was %s", __func__, buf);
		return def;
	}
	return ret;
}
