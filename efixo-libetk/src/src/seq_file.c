/*!
 * \file lib/seq_file.c
 *
 * \author Copyright 2007 Gaio Miguel <miguel.gaio@efixo.com>
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
 * $Id: seq_file.c 14618 2010-02-05 13:24:52Z mgo $
 */
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <netinet/ether.h>
#include <arpa/inet.h>

#include <etk/common.h>
#include <etk/log.h>
#include <etk/string.h>
#include <etk/seq_file.h>

int seq_printf(struct seq_file *m, const char *f, ...)
{
	va_list args;
	int len;

	if (m->count < m->size) {
		va_start(args, f);
		len = vsnprintf(m->buf + m->count, m->size - m->count, f, args);
		va_end(args);
		if (m->count + len < m->size) {
			m->count += len;
			return 0;
		}
	}
	m->count = m->size;
	return -1;
}

void seq_print_hex(struct seq_file *m, uint8_t const *bin, size_t len)
{
	uint8_t const *p = bin;
	uint8_t const *end = bin + len;
	size_t const max_count = m->size - 1;

	len = min(len, m->size - m->count);
	end = bin + len;
	for (char *to = m->buf + m->count;
	     (p < end) && (m->count < max_count); ++to, ++p) {
		*to = hex_ascii(*p >> 4);
		++m->count;
		*(++to) = hex_ascii(*p & 0x0F);
		++m->count;
	}

	m->buf[min(m->count, max_count)] = '\0';
}

void seq_print_mac_addr(struct seq_file *m, uint8_t const *mac,
			enum mac_addr_format fmt, int off)
{
	struct ether_addr e;
	uint32_t last8digit;

	memcpy(e.ether_addr_octet, mac, sizeof(e));

	memcpy(&last8digit, e.ether_addr_octet + 2, sizeof(last8digit));
	last8digit = ntohl(last8digit);
	last8digit += off;
	last8digit = htonl(last8digit);
	memcpy(e.ether_addr_octet + 2, &last8digit, sizeof(last8digit));

	switch (fmt) {
	case mac_addr_format_voip:
		seq_printf(m, "%02x-%02x-%02x-%02x-%02x-%02x",
			   e.ether_addr_octet[0], e.ether_addr_octet[1],
			   e.ether_addr_octet[2], e.ether_addr_octet[3],
			   e.ether_addr_octet[4], e.ether_addr_octet[5]);
		break;

	case mac_addr_format_compact:
		seq_printf(m, "%02X%02X%02X%02X%02X%02X",
			   e.ether_addr_octet[0], e.ether_addr_octet[1],
			   e.ether_addr_octet[2], e.ether_addr_octet[3],
			   e.ether_addr_octet[4], e.ether_addr_octet[5]);
		break;

	default:
	case mac_addr_format_system:
		seq_printf(m, "%02X:%02X:%02X:%02X:%02X:%02X",
			   e.ether_addr_octet[0], e.ether_addr_octet[1],
			   e.ether_addr_octet[2], e.ether_addr_octet[3],
			   e.ether_addr_octet[4], e.ether_addr_octet[5]);
		break;

	}
}

int seq_print_net(struct seq_file *m, char const *s_ipaddr,
		  char const *s_netmask)
{
	struct in_addr ipaddr;
	struct in_addr netmask;
	struct in_addr network;
	int mask = 32;

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

	mask -= ffs((int)netmask.s_addr) - 1;
	if (mask > 32) {	/* maybe someone can set netmask to 0.0.0.0 ? */
		mask = 0;
	}

	network.s_addr = htonl(ipaddr.s_addr & netmask.s_addr);

	return seq_printf(m, "%s/%d", inet_ntoa(network), mask);
}
