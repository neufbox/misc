
#include <stdio.h>
#include <sys/types.h>

char const *http_encode(char const *from, char *to, size_t len)
{
	int n;
	char *__to = to;
	char c;

	while (((c = *from) != '\0') && len) {
		++from;
		if (c == ' ') {
			n = snprintf(to, len, "%%20");
			len -= n, to += n;
		} else {
			*to = c;
			--len, ++to;
		}
	}
	if (len == 0)
		--to;
	*to = '\0';

	return __to;
}
