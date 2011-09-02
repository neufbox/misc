#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void ewf_log_write(const char *filename, const char *fmt, ...)
{
	va_list args;
	char buf[64];

	va_start(args, fmt);

	ewf_string_vprintf(buf, sizeof buf, fmt, args);
	printf("%s", buf);

	va_end(args);

	return;
}

int main(int argc, char **argv)
{
	ewf_log_write("ewf_string_log.c", "%s:%s:%d\n", "test", "toto", 8);

	return 0;
}
