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
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* -- */
#include <etk/log.h>
#include <etk/socket.h>

char const *socket_print(struct socket const *sk)
{
	static char buf[256];
	char hbuf[196], sbuf[32];
	struct sockaddr const *sa = &sk->u.sk;
	socklen_t len = sizeof(sk->u);

	buf[0] = '\0';
	if (!getnameinfo(sa, len, hbuf, sizeof(hbuf), sbuf,
			 sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV))
		snprintf(buf, sizeof(buf), "%s:%s", hbuf, sbuf);

	return buf;
}

void socket_disconnect(struct socket *sk)
{
	if (sk->fd > 0) {
		trace("%s: %-32s %s", "socket", socket_print(sk),
		      "disconnected");

		close(sk->fd);
		sk->fd = -1;
		if (is_unix(sk))
			unlink(sk->u.un.sun_path);
	}
}
