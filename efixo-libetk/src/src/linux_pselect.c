
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

/* Linux */
#include <sys/syscall.h>

int linux_pselect(int nfds, fd_set * readfds, fd_set * writefds,
		  fd_set * exceptfds, const struct timespec *to,
		  const sigset_t * sigmask)
{
	/* The Linux kernel can in some situations update the to value.
	 * We do not want that so use a local variable.  */
	struct timespec tval;
	if (to != NULL) {
		tval = *to;
		to = &tval;
	}

	/* Note: the system call expects 7 values but on most architectures
	 * we can only pass in 6 directly.  If there is an architecture with
	 * support for more parameters a new version of this file needs to
	 * be created.  */
	struct {
		const sigset_t *ss;
		size_t ss_len;
	} data;

	data.ss = sigmask;
	data.ss_len = _NSIG / 8;

	return syscall(SYS_pselect6, nfds, readfds, writefds, exceptfds,
		       to, &data);
}
