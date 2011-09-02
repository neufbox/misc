/*!
 * \file etk/net.h
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

#ifndef _ETK_NET_H_
#define _ETK_NET_H_ 1

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct ether_addr;

int ip_mc_ethernet(const char *, char *, size_t);
int ip_sprint(char *, size_t, char const *, char const *);
int net_sprint(char *, size_t, char const *, char const *);
int broadcast_sprint(char *, size_t, char const *, char const *);

int arp_mac_from_ip(char const *, char const *, char *, size_t);
int arp_ip_from_mac(char const *, char const *, char *, size_t);

static inline int arp_add_entry(char const *ifname, char const *ip_addr,
				char const *mac_addr)
{
	extern int __arp_add_del_entry(char const *, char const *, char const *,
				       int);

	return __arp_add_del_entry(ifname, ip_addr, mac_addr, 1);
}

static inline int arp_del_entry(char const *ifname, char const *ip_addr,
				char const *mac_addr)
{
	extern int __arp_add_del_entry(char const *, char const *, char const *,
				       int);

	return __arp_add_del_entry(ifname, ip_addr, mac_addr, 0);
}

void dns_setup_local(void);
int ip_getaddrinfo(char const *host, char const *service,
			   struct addrinfo **res);
int local_gethostname(char const *s, char *buf, size_t len);
int bridge_if_indextoname(char const *, unsigned, char *);
int net_bridge_port(char const *, struct ether_addr *, char *, size_t);

static inline int ipv4_is_ssdp(char const *mcast)
{
	struct in_addr addr;

	inet_aton(mcast, &addr);

	return (addr.s_addr == htonl(0xeffffffa));
}

static inline int ipv4_is_local_multicast(char const *mcast)
{
	struct in_addr addr;

	inet_aton(mcast, &addr);

	return (addr.s_addr & htonl(0xffffff00)) == htonl(0xe0000000);
}

#endif
