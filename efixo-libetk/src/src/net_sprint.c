/*!
 * \file .c
 *
 * \brief  Implement utils
 *
 * \author Copyright 2007 Miguel GAIO <miguel.gaio@efixo.com>
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
 * 
 * $Id: net_sprint.c 15371 2010-03-25 09:56:02Z mgo $
 */

#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <stdio.h>
#include <syslog.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <etk/log.h>

int ip_mc_ethernet(const char *mc_ip_addr, char *mc_ethernet, size_t len)
{
	struct in_addr ip_mc;

	if (inet_aton(mc_ip_addr, &ip_mc) == 0) {
		err("%s", mc_ip_addr);
		return -1;
	}

	snprintf(mc_ethernet, len, "01:00:5E:%02X:%02X:%02X",
		 (ip_mc.s_addr >> 16) & 0x7F,
		 (ip_mc.s_addr >> 8) & 0xFF, (ip_mc.s_addr >> 0) & 0xFF);

	return 0;
}

static int ipnet_sprint(char *buf, size_t len, char const *s_ipaddr,
			char const *s_netmask, bool print_ip)
{
	struct in_addr ipaddr;
	struct in_addr netmask;
	unsigned int mask = 32U;

	/* process IP address */
	if (!inet_aton(s_ipaddr, &ipaddr)) {
		err("invalid ip address: %s", s_ipaddr);
		return 0;
	}

	/* process netmask */
	if (!inet_aton(s_netmask, &netmask)) {
		err("invalid netmask: %s", s_netmask);
		return 0;
	}

	/* Valid IP address and netmask */
	ipaddr.s_addr = ntohl(ipaddr.s_addr);
	netmask.s_addr = ntohl(netmask.s_addr);

	mask -= ffs(netmask.s_addr) - 1U;
	if (mask > 32U) {	/* maybe someone can set netmask to 0.0.0.0 ? */
		mask = 0U;
	}

	if (print_ip) {
		return snprintf(buf, len, "%s/%u", inet_ntoa(ipaddr), mask);
	} else {
		struct in_addr network;

		network.s_addr = htonl(ipaddr.s_addr & netmask.s_addr);
		return snprintf(buf, len, "%s/%u", inet_ntoa(network), mask);
	}
}

int ip_sprint(char *buf, size_t len, char const *ipaddr, char const *netmask)
{
	return ipnet_sprint(buf, len, ipaddr, netmask, true);
}

int net_sprint(char *buf, size_t len, char const *ipaddr, char const *netmask)
{
	return ipnet_sprint(buf, len, ipaddr, netmask, false);
}

int broadcast_sprint(char *buf, size_t len, char const *s_ipaddr,
		     char const *s_netmask)
{
	struct in_addr ipaddr;
	struct in_addr netmask;

	/* process IP address */
	if (!inet_aton(s_ipaddr, &ipaddr)) {
		err("invalid ip address: %s", s_ipaddr);
		return 0;
	}

	/* process netmask */
	if (!inet_aton(s_netmask, &netmask)) {
		err("invalid netmask: %s", s_netmask);
		return 0;
	}

	ipaddr.s_addr = htonl(ipaddr.s_addr & (~netmask.s_addr));

	return snprintf(buf, len, "%s", inet_ntoa(ipaddr));
}
