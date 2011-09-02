
#include <stdio.h>

#include <sys/types.h>
#include <signal.h>

#include <etk/log.h>

int wrapper_wakeup(char const *wrapper)
{
	char pidfile[128];
	FILE *fp;
	pid_t pid = -1;

	snprintf(pidfile, sizeof(pidfile), "/var/run/%s-wrapper.pid", wrapper);
	if ((fp = fopen(pidfile, "r")) == NULL)
		return -1;

	if (fscanf(fp, "%d", &pid) != 1)
		warn("%s invalid", pidfile);
	fclose(fp);

	if (pid > 0) {
		kill(pid, SIGHUP);
		return 0;
	}

	return -1;
}
