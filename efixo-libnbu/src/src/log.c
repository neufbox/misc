#include "nbu/log.h"

#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>

#include "nbu/nbu.h"

void nbu_log_open(const char *ident)
{
	openlog(ident, LOG_PID, LOG_DAEMON);
	
	return;
}

void nbu_log_close(void)
{
	closelog();
	
	return;
}

void nbu_log_write(int level, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	vsyslog(level, fmt, args);
        
	va_end(args);

	return;
}
