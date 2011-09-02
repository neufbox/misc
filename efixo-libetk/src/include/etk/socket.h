/*!
 * \author Copyright 2010 Miguel GAIO <miguel.gaio@efixo.com>
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

#ifndef _ETK_SOCKET_H_
#define _ETK_SOCKET_H_ 1

#include <fcntl.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <linux/netlink.h>

struct socket {
	union sk {
		struct sockaddr sk;
		struct sockaddr_un un;
		struct sockaddr_in in;
		struct sockaddr_in6 in6;
		struct sockaddr_nl nl;
	} u;
	int fd;
};

char const *socket_print(struct socket const *);

int unix_listen(struct socket *sk, char const *path);
int inet_listen(struct socket *sk, char const *port);

static inline int socket_listen(struct socket *sk, char const *s)
{
	if (!memcmp(s, "::", sizeof("::") - 1))
		return inet_listen(sk, s + sizeof("::") - 1);
	else
		return unix_listen(sk, s);
}

void socket_disconnect(struct socket *);

static inline void inet_close(struct socket * sk)
{
	socket_disconnect(sk);
}

static inline void unix_close(struct socket * sk)
{
	socket_disconnect(sk);
}

static inline int is_open(int fd)
{
	return (fd > 0);
}

static inline int is_unix(struct socket const *sk)
{
	return (sk->u.sk.sa_family == AF_UNIX);
}

static inline int is_ipv4(struct socket const *sk)
{
	return (sk->u.sk.sa_family == AF_INET);
}

static inline int is_ipv6(struct socket const *sk)
{
	return (sk->u.sk.sa_family == AF_INET6);
}

static inline int is_netlink(struct socket const *sk)
{
	return (sk->u.sk.sa_family == AF_NETLINK);
}

static inline int setcloseonexec(int fd)
{
	return fcntl(fd, F_SETFD, FD_CLOEXEC);
}

static inline int setnonblocking(int fd)
{
	return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

#endif
