/*!
 * \file spy-send.c
 *
 * \brief  Implement server interface
 *
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netdb.h>

#include <etk/seq_file.h>
#include <etk/socket.h>

#define fatal(fmt, ...) \
do { \
	fprintf(stderr,"%s "fmt"\n", __func__, ##__VA_ARGS__); \
	exit(EXIT_FAILURE); \
} while (0)

#define warn(fmt, ...) \
do { \
	fprintf(stderr,"%s "fmt"\n", __func__, ##__VA_ARGS__); \
} while (0)

#define dbg(fmt, ...) \
do { \
	printf(fmt"\n", ##__VA_ARGS__); \
} while (0)

#define SYSLOG_DEFAULT_SERVER "stats."OPERATOR_DOMAIN

static char const *ai_print(struct addrinfo *ai, char *buf, size_t len)
{
	char hbuf[196], sbuf[32];

	buf[0] = '\0';
	if (!getnameinfo(ai->ai_addr, ai->ai_addrlen, hbuf, sizeof(hbuf), sbuf,
			 sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV))
		snprintf(buf, len, "%s:%s", hbuf, sbuf);

	return buf;
}

static ssize_t spy_ai_send(struct addrinfo *ai, struct seq_file const *m)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	ssize_t len = -1;
	char from[128];
	char to[128];
	int fd;
	int tos;
	int s;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = ai->ai_family;
	hints.ai_socktype = ai->ai_socktype;
	hints.ai_flags = AI_PASSIVE;
	s = getaddrinfo(NULL, "syslog", &hints, &result);
	if (s != 0)
		fatal("getaddrinfo: %s\n", gai_strerror(s));

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if ((fd = socket(rp->ai_family, rp->ai_socktype,
				 rp->ai_protocol)) < 0)
			fatal("%s(%s) %m", "socket",
			      ai_print(rp, from, sizeof(from)));

		if (setcloseonexec(fd) < 0)
			fatal("%s(%s) %m", "setcloseonexec",
			      ai_print(rp, from, sizeof(from)));

		tos = (VOIP_DSCP << 2);
		setsockopt(fd, SOL_IP, IP_TOS, &tos, sizeof(tos));

		if (bind(fd, rp->ai_addr, rp->ai_addrlen) < 0)
			fatal("%s(%s) %m", "bind",
			      ai_print(rp, from, sizeof(from)));

		len = sendto(fd, seq_c_str(m), seq_length(m), 0, ai->ai_addr,
			     ai->ai_addrlen);

		dbg("%s > %s: %s",
		    ai_print(rp, from, sizeof(from)),
		    ai_print(ai, to, sizeof(to)), seq_c_str(m));
		if (len <= 0) {
			warn("%s(%s -> %s) %m", "sendto",
			     ai_print(rp, from, sizeof(from)),
			     ai_print(ai, to, sizeof(to)));
			close(fd);
			continue;
		}

		close(fd);
		break;
	}

	freeaddrinfo(result);

	return len;
}

static void spy_send(struct seq_file const *m)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	char to[128];
	int s;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	s = getaddrinfo(SYSLOG_DEFAULT_SERVER, "syslog", &hints, &result);
	if (s != 0)
		fatal("getaddrinfo: %s\n", gai_strerror(s));

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if (spy_ai_send(rp, m) < 0)
			continue;
	}

	freeaddrinfo(result);
}

static void spy_message(struct seq_file *m, char const *msg)
{
	char buf[32];
	struct tm *tm;
	time_t now;

	time(&now);
	if (!(tm = localtime(&now))) {
		snprintf(buf, sizeof(buf), "%ld", (long)now);
	} else {
		strftime(buf, sizeof(buf), "%h %e %H:%M:%S", tm);
	}
	buf[16] = '\0';
	seq_printf(m, "<26>%s ", buf);
	gethostname(buf, sizeof(buf));
	seq_printf(m, "%s nbd: %s", buf, msg);
}

int main(int argc, char *argv[])
{
	char buf[512];
	DEFINE_SEQFILE(m, buf, sizeof(buf));

	if (argc < 2) {
		fatal("usage: %s <message>\n", argv[0]);
	}

	spy_message(&m, argv[1]);
	spy_send(&m);

	return 0;
}
