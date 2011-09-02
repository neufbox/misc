/*!
 * \author Copyright 2009 Miguel GAIO <miguel.gaio@efixo.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* ISO C*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>

/* POSIX */
#include <unistd.h>
#include <syscall.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>

/* -- */
#include <etk/log.h>
#include <etk/socket.h>

int unix_listen(struct socket *sk, char const *path)
{
	struct sockaddr_un *un = &sk->u.un;
	socklen_t sklen = sizeof(*un);
	int fd;

	memset(sk, 0x00, sizeof(*sk));
	un->sun_family = AF_UNIX;
	snprintf(un->sun_path, sizeof(un->sun_path), "%s", path);

	if ((fd = socket(un->sun_family, SOCK_STREAM, 0)) < 0) {
		fatal("%s(%s)", "socket", socket_print(sk));
		close(fd);
		return -1;
	}

	if (setcloseonexec(fd) < 0) {
		fatal("%s(%s)", "setcloseonexec", socket_print(sk));
		close(fd);
		return -1;
	}

	if (setnonblocking(fd) < 0) {
		fatal("%s(%s)", "setnonblocking", socket_print(sk));
		close(fd);
		return -1;
	}

	if (bind(fd, &sk->u.sk, sklen) < 0) {
		fatal("%s(%s)", socket_print(sk), socket_print(sk));
		close(fd);
		return -1;
	}

	if (listen(fd, 10) < 0) {
		fatal("%s(%s, %d)", "listen", socket_print(sk), 5);
		close(fd);
		return -1;
	}

	sk->fd = fd;

	trace("%s: %-32s %s", "socket", socket_print(sk), "connected");

	return fd;
}
