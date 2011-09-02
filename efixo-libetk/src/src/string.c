
#include <string.h>

char const *strrtrim(char *str)
{
	char *end;

	end = str + strlen(str);
	while (end-- > str) {
		if (!strchr(" \t\n\r", *end))
			return str;
		*end = 0;
	}
	return str;
}

char const *strltrim(char const *str)
{
	while (*str) {
		if (!strchr(" \t\n\r", *str))
			return str;
		++str;
	}
	return str;
}
