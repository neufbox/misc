
#include <stdlib.h>
#include <unistd.h>

#include <syslog.h>

#include <etk/string.h>

int syslog_level = LOG_NOTICE;

void setup_log_trigger(void)
{
	char const *level = getenv("LOG_LEVEL");

	if (level == NULL)
		return;

	/*
	 *  LOG_EMERG system is unusable
	 *  LOG_ALERT action must be taken immediately
	 *  LOG_CRIT critical conditions
	 *  LOG_ERR error conditions
	 *  LOG_WARNING warning conditions
	 *  LOG_NOTICE normal, but significant, condition
	 *  LOG_INFO informational message
	 *  LOG_DEBUG
	 */
	if (matches(level, "emerg"))
		syslog_level = LOG_EMERG;
	else if (matches(level, "alert"))
		syslog_level = LOG_ALERT;
	else if (matches(level, "crit"))
		syslog_level = LOG_CRIT;
	else if (matches(level, "err"))
		syslog_level = LOG_ERR;
	else if (matches(level, "warning"))
		syslog_level = LOG_WARNING;
	else if (matches(level, "notice"))
		syslog_level = LOG_NOTICE;
	else if (matches(level, "info"))
		syslog_level = LOG_INFO;
	else if (matches(level, "debug"))
		syslog_level = LOG_DEBUG;
}
