
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include <sys/socket.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <resolv.h>

#include <etk/log.h>

int ip_getaddrinfo(char const *ipaddr, char const *service,
		   struct addrinfo **res)
{
	struct addrinfo hints;
	int e;

	memset(&hints, '\0', sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICHOST;

	e = getaddrinfo(ipaddr, service, &hints, res);
	if (e) {
		err("%s(%s:%s) failed: %s", "getaddrinfo",
		    ipaddr, service, gai_strerror(e));
		return -1;
	}

	return 0;
}

int local_gethostname(char const *ipaddr, char *buf, size_t len)
{
	struct addrinfo *res = NULL, *rp = NULL;
	int e;

	ip_getaddrinfo(ipaddr, NULL, &res);

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		e = getnameinfo((const struct sockaddr *)rp->ai_addr,
				rp->ai_addrlen, buf, len, NULL, 0, NI_NAMEREQD);
		if (!e) {
			freeaddrinfo(res);
			return 0;
		}
	}

	freeaddrinfo(res);

	return -1;
}

void dns_setup_local(void)
{
	struct addrinfo *res = NULL, *rp = NULL;

	static bool once = false;
	if (once) {
		once = true;
		return;
	}

	res_init();

	_res._u._ext.nscount = 0;
	_res.nscount = 0;

#if 0
	/* IPv6 */
	ip_getaddrinfo("::1", "domain", &res);
	for (rp = res; rp != NULL; rp = rp->ai_next) {
		_res._u._ext.nsaddrs[0] = (void *)&rp->ai_addr;
		++_res._u._ext.nscount;

	}
#endif

	/* IPv4 */
	ip_getaddrinfo("127.0.0.1", "domain", &res);
	for (rp = res; rp != NULL; rp = rp->ai_next) {
		_res.nsaddr_list[_res.nscount] =
		    *(struct sockaddr_in *)rp->ai_addr;
		++_res.nscount;
	}
}
