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

/* POSIX */
#include <unistd.h>
#include <syscall.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

/* -- */
#include <etk/log.h>
#include <etk/socket.h>

int inet_listen(struct socket *sk, char const *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int fd, err;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	if ((err = getaddrinfo(NULL, port, &hints, &result)))
		fatal("%s(%s) %s", "getaddrinfo", port, gai_strerror(err));

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		memcpy(sk, rp->ai_addr, rp->ai_addrlen);

		if ((fd = socket(rp->ai_family, rp->ai_socktype,
				 rp->ai_protocol)) < 0)
			fatal("%s(%s)", "socket", socket_print(sk));

		if (setcloseonexec(fd) < 0)
			fatal("%s(%s)", "setcloseonexec", socket_print(sk));

		if (setnonblocking(fd) < 0)
			fatal("%s(%s)", "setnonblocking", socket_print(sk));

		if (bind(fd, rp->ai_addr, rp->ai_addrlen) < 0)
			fatal("%s(%s)", "bind", socket_print(sk));

		if (listen(fd, 10) < 0)
			fatal("%s(%s)", "listen", socket_print(sk));

		sk->fd = fd;

		trace("%s: %-32s %s", "socket", socket_print(sk), "connected");
	}

	freeaddrinfo(result);

	return 0;
}
