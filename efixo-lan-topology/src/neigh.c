
#include <stdbool.h>
#include <stdio.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <netlink/socket.h>
#include <netlink/netlink.h>
#include <netlink/msg.h>

#include <ezxml.h>

#include <etk/common.h>
#include <etk/log.h>
#include <etk/string.h>
#include <etk/exec.h>
#include <etk/net.h>

#include <nbd/core.h>
#include <nbd/nvram.h>
#include <nbd/lan.h>

#include "plugin.h"

#define NEIGH_TIMEOUT 1200

static int neigh_init(struct plugin *plugin);
static void neigh_cleanup(struct plugin *plugin);
static int neigh_poll(struct plugin *plugin);
static int neigh_event(struct plugin *plugin);

struct plugin neigh_plugin = {
	.name = "neigh",
	.init = neigh_init,
	.cleanup = neigh_cleanup,
	.poll = neigh_poll,
	.event = neigh_event,
};

extern struct in_addr lan_network;
extern struct in_addr lan_netmask;
extern unsigned long now;
extern bool debug;

struct nl_sock *sk;

static void hexdump(unsigned char *data, size_t len)
{
	int c, l;
	int end;

	for (c = 0; len; ++c) {

		if (len > 16) {
			end = 16;
			len -= 16;
		} else {
			end = len;
			len = 0;
		}

		for (l = 0; l < end; ++l) {
			if (l == 8)
				printf(" ");
			printf(" %02x", data[(16 * c) + l]);
		}
		printf("\n");
	}
}

#if 0
struct nlmsghdr {
	__u32 nlmsg_len;	/* Length of message including header */
	__u16 nlmsg_type;	/* Message content */
	__u16 nlmsg_flags;	/* Additional flags */
	__u32 nlmsg_seq;	/* Sequence number */
	__u32 nlmsg_pid;	/* Sending process port ID */
};

struct ndmsg {
	__u8 ndm_family;
	__u8 ndm_pad1;
	__u16 ndm_pad2;
	__s32 ndm_ifindex;
	__u16 ndm_state;
	__u8 ndm_flags;
	__u8 ndm_type;
};
#endif

static int my_rcv_debug(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	unsigned char *data = nlmsg_data(nlh);
	struct ndmsg *nln = (void *)data;
	unsigned char *ip = data + 16;
	unsigned char *ether = ip + 8;

	printf("\n--\n");
	printf("nlh: len:%u type:%u flags:%u seq:%u pid:%u\n\n",
	       nlh->nlmsg_len,
	       nlh->nlmsg_type,
	       nlh->nlmsg_flags, nlh->nlmsg_seq, nlh->nlmsg_pid);
	hexdump(data, nlh->nlmsg_len);
	printf("nln: famliy:%u ifindex:%u state:%u flags:%u type:%u\n",
	       nln->ndm_family,
	       nln->ndm_ifindex, nln->ndm_state, nln->ndm_flags, nln->ndm_type);

	printf
	    ("ip:%u.%u.%u.%u ether:%02x:%02x:%02x:%02x:%02x:%02x [%s%s%s%s%s%s]\n",
	     ip[0], ip[1], ip[2], ip[3], ether[0], ether[1], ether[2], ether[3],
	     ether[4], ether[5],
	     (nln->ndm_state & NUD_INCOMPLETE) ? " INCOMPLETE" : " ",
	     (nln->ndm_state & NUD_REACHABLE) ? " REACHABLE" : " ",
	     (nln->ndm_state & NUD_STALE) ? " STALE" : " ",
	     (nln->ndm_state & NUD_DELAY) ? " DELAY" : " ",
	     (nln->ndm_state & NUD_PROBE) ? " PROBE" : " ",
	     (nln->ndm_state & NUD_FAILED) ? " FAILED" : " ");
	return NL_OK;
}

static int my_rcv(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	unsigned char *data = nlmsg_data(nlh);
	struct ndmsg *nln = (void *)data;
	unsigned char *ip = data + 16;
	unsigned char *ether = ip + 8;
	struct in_addr host_ipaddr;

	if (debug)
		my_rcv_debug(msg, arg);

	if (nln->ndm_state &
	    (NUD_REACHABLE | NUD_STALE | NUD_DELAY | NUD_FAILED)) {
		char macaddr[32] = "00:00:00:00:00:00";
		char ipaddr[16];

		memcpy(&host_ipaddr, ip, sizeof(ipaddr));
		if ((host_ipaddr.s_addr & lan_netmask.s_addr) !=
		    lan_network.s_addr)
			return NL_OK;

		if (!(nln->ndm_state & NUD_FAILED)) {
			snprintf(macaddr, sizeof(macaddr),
				 "%02x:%02x:%02x:%02x:%02x:%02x", ether[0],
				 ether[1], ether[2], ether[3], ether[4],
				 ether[5]);
		}

		inet_ntop(AF_INET, &host_ipaddr, ipaddr, sizeof(ipaddr));

		nbd_open();
		nbd_lan_topology_update(macaddr, ipaddr, NULL);
		nbd_close();
	}

	return NL_OK;
}

static int neigh_init(struct plugin *plugin)
{
	sk = nl_socket_alloc();
	if (!sk) {
		sys_err("%s()", "nl_socket_alloc");
		return -1;
	}

	nl_join_groups(sk, 1 << (RTNLGRP_NEIGH - 1));

	if (nl_connect(sk, NETLINK_ROUTE) < 0) {
		sys_err("%s()", "nl_connect");
		return -1;
	}

	nl_cb_set_all(sk->s_cb, NL_CB_CUSTOM, my_rcv, NULL);

	plugin->fd = nl_socket_get_fd(sk);
	return 0;
}

static void neigh_cleanup(struct plugin *plugin)
{
	nl_close(sk);
	nl_socket_free(sk);
}

static int neigh_poll(struct plugin *plugin)
{
	char lan_ifname[16];
	char *s;
	size_t len;
	ezxml_t xml, node;
	unsigned long activity;

	nbd_open();
	nbd_query_new(&s, &len, "status", "xml", "lan_topology", NULL);
	if (!s) {
		err("%s(%s)", "nbd_status_xml", "lan_topology");
		return -1;
	}
	nbd_system_config_get("lan_ifname", lan_ifname, sizeof(lan_ifname));
	nbd_close();

	xml = ezxml_parse_str(s, strlen(s));
	if (!xml) {
		err("%s(%s)", "ezxml_parse_str", "lan_topology");
		free(s);
		return -1;
	}

	for (node = ezxml_child(xml, "device"); node; node = node->ordered) {
		if (matches(ezxml_child_txt(node, "type"), "plc"))
			continue;

		activity = safe_strtol(ezxml_child_txt(node, "alive"), 0L);
		if (time_after_eq(now, activity + NEIGH_TIMEOUT)) {
			char const *macaddr = ezxml_child_txt(node, "macaddr");
			char const *ipaddr = ezxml_child_txt(node, "ipaddr");

			info("ARP probe on %s %s/%s", lan_ifname, macaddr,
			     ipaddr);
			exec("ip", "neigh", "add", ipaddr, "lladdr", macaddr,
			     "dev", lan_ifname, "nud", "probe");
		}
	}

	ezxml_free(xml);
	free(s);

	return 0;
}

static int neigh_event(struct plugin *plugin)
{
	return nl_recvmsgs_default(sk);
}
