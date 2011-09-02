/*!
 * \file plugins/plc.c
 *
 * \author Copyright 2006 Miguel GAIO <miguel.gaio@efixo.com>
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
 * $Id: plc.c 15978 2010-05-06 12:55:35Z mgo $
 */

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <net/if.h>

#include <etk/common.h>
#include <etk/log.h>
#include <etk/socket.h>
#include <etk/io.h>
#include <etk/seq_file.h>
#include <etk/mme.h>

#include <nbd/core.h>
#include <nbd/plc.h>

#include "plugin.h"

static int plc_init(struct plugin *plugin);
static void plc_cleanup(struct plugin *plugin);
static int plc_poll(struct plugin *plugin);
static int plc_event(struct plugin *plugin);

struct plugin plc_plugin = {
	.name = "plc",
	.init = plc_init,
	.cleanup = plc_cleanup,
	.poll = plc_poll,
	.event = plc_event,
};

extern char lan_ifname[16];
struct ether_addr lan_ethaddr;

static int open_netdevice(char const *dev, struct ether_addr *addr,
			  __be16 ether_type)
{
	struct ifreq ifr;
	struct sockaddr_ll sll;
	int fd;

	if ((fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		sys_err("%s(%s, %s, %s)", "socket", "PF_PACKET",
			"SOCK_RAW", "ETH_P_ALL");
		return -1;
	}

	setcloseonexec(fd);

	strncpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
		sys_err("%s(%s:%s)", "ioctl", dev, "SIOCGIFHWADDR");
		close(fd);
		return -1;
	}

	memcpy(addr, ifr.ifr_hwaddr.sa_data, sizeof(*addr));

	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
		sys_err("%s(%s:%s)", "ioctl", dev, "SIOCGIFINDEX");
		close(fd);
		return -1;
	}

	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifr.ifr_ifindex;
	sll.sll_protocol = ether_type;

	if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
		sys_err("%s(%s:%s)", "bind", dev, "AF_PACKET");
		close(fd);
		return -1;
	}

	return fd;
}

static int mme_request(int fd, void const *ether_shost,
		       void const *ether_dhost, uint16_t query)
{
	char frame[ETH_FRAME_LEN];
	struct ether_header *eth_hdr;
	struct mme_header *mme_hdr;
	struct homeplug_info const *hpav;
	size_t len = 0;

	memset(frame, 0x00, sizeof(frame));
	/* build ethernet header */
	eth_hdr = (void *)(frame + len);
	memcpy(eth_hdr->ether_shost, ether_shost, sizeof(eth_hdr->ether_shost));
	memcpy(eth_hdr->ether_dhost, ether_dhost, sizeof(eth_hdr->ether_dhost));
	eth_hdr->ether_type = htons(0x88e1);	/* HomePlug frame */
	len += sizeof(*eth_hdr);

	/* build HomePlug header */
	mme_hdr = (void *)(frame + len);
	memset(mme_hdr, 0x00, sizeof(*mme_hdr));
	mme_hdr->mme_v = 0U;
	mme_hdr->mme_type = cpu_to_le16(query);	/* discover */
	mme_hdr->mme_oui[0] = 0x00;
	mme_hdr->mme_oui[1] = 0xB0;
	mme_hdr->mme_oui[2] = 0x52;
	len += sizeof(*mme_hdr);

	hpav = (void const *)(frame + len);
	len = max((int)len, ETHER_MIN_LEN);

	if (write(fd, frame, len) < 0) {
		fatal("%s()", "write");
		return -1;
	}

	return 0;
}

static int mme_probe(int fd)
{
	struct ether_addr eth_addr;

	/* probe local plugs info */
	ether_aton_r("00:b0:52:00:00:01", &eth_addr);
	mme_request(fd, &lan_ethaddr, &eth_addr, 0xa000);
	mme_request(fd, &lan_ethaddr, &eth_addr, 0xa038);

	return 0;
}

static int mme_discover(int fd)
{
	char frame[ETH_FRAME_LEN];
	struct ether_header const *const eth_hdr = (void const *)frame;
	struct mme_header const *const mme =
	    (void const *)((char const *)eth_hdr + sizeof(*eth_hdr));
	void const *const data =
	    (void const *)((char const *)mme + sizeof(*mme));

	int i, j;

	if (read(fd, frame, sizeof(frame)) <= 0) {
		return -1;
	}

	if (eth_hdr->ether_type != htons(0x88e1)) {
		return 0;
	}

	if (mme->mme_type == cpu_to_le16(0xa039)) {
		struct hpav_networks const *const networks = data;
		/* fetch remote plug info */

		for (i = 0; i < networks->num; ++i) {
			struct hpav_network const *const network =
			    &networks->network[i];

			for (j = 0; j < network->num; ++j) {
				struct hpav_station const *const station
				    = &network->station[j];
				mme_request(fd, &lan_ethaddr, station->MAC,
					    0xa000);
			}
		}
	} else if (mme->mme_type == cpu_to_le16(0xa001)) {
		char macaddr[sizeof("00:00:00:00:00:00")];
		struct hpav_info const *const info = data;

		snprintf(macaddr, sizeof(macaddr),
			 "%02x:%02x:%02x:%02x:%02x:%02x",
			 eth_hdr->ether_shost[0],
			 eth_hdr->ether_shost[1],
			 eth_hdr->ether_shost[2],
			 eth_hdr->ether_shost[3],
			 eth_hdr->ether_shost[4], eth_hdr->ether_shost[5]);
		nbd_open();
		nbd_plc_add(macaddr, info->version);
		nbd_close();
	}

	return 0;
}

static int plc_init(struct plugin *plugin)
{
	plugin->fd = open_netdevice(lan_ifname, &lan_ethaddr, htons(0x88e1));
	return mme_probe(plugin->fd);
}

static void plc_cleanup(struct plugin *plugin)
{
	close(plugin->fd);
}

static int plc_poll(struct plugin *plugin)
{
	return mme_probe(plugin->fd);
}

static int plc_event(struct plugin *plugin)
{
	return mme_discover(plugin->fd);
}
