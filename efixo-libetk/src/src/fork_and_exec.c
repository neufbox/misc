
#include <stdlib.h>
#include <signal.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <etk/log.h>

int fork_and_exec(char const *const argv[], int nowait)
{
	pid_t pid;
	int status = 0;

	pid = fork();
	if (pid < 0) {
		sys_err("%s %s %s", "fork", argv[0], argv[1]);
		return -1;
	}

	if (!pid) {
		sigset_t unblocked;

		/* sanitized signal mask */
		sigemptyset(&unblocked);
		sigprocmask(SIG_SETMASK, &unblocked, NULL);
		execvp(argv[0], (char **)(unsigned long)argv);
		sys_err("%s(%s, %s)", "execvp", argv[0], argv[1]);
		exit(EXIT_FAILURE);
	}

	if (!nowait)
		waitpid(pid, &status, 0);

	return status;
}
