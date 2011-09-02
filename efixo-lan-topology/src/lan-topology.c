
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

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

#include <etk/common.h>
#include <etk/log.h>
#include <etk/string.h>
#include <etk/net.h>
#include <etk/linux.h>

#include <nbd/core.h>
#include <nbd/nvram.h>
#include <nbd/lan.h>

#include "plugin.h"

#define POLL_INTERVAL 300

LIST_HEAD(plugin_list);

unsigned long now = 0UL;
char lan_ifname[16];
struct in_addr lan_network;
struct in_addr lan_netmask;
bool debug = false;

static void setup(int argc, char *argv[])
{
	char ipaddr[16];
	char netmask[16];
	struct in_addr lan_ipaddr;

	if (argc == 2) {
		debug = matches(argv[1], "--debug");
	}

	if (!debug) {
		if (daemon(0, 0) < 0) {
			sys_err("%s()", "daemon");
			exit(EXIT_FAILURE);
		}
	} else {
		setenv("LOG_LEVEL", "debug", 1);
	}

	setup_log_trigger();
	openlog(argv[0], LOG_PID, LOG_DAEMON);

	nbd_open();
	nbd_user_config_get("lan_ipaddr", ipaddr, sizeof(ipaddr));
	nbd_user_config_get("lan_netmask", netmask, sizeof(netmask));
	nbd_user_config_get("lan_ifname", lan_ifname, sizeof(lan_ifname));
	nbd_close();

	/* process IP address */
	if (!inet_aton(ipaddr, &lan_ipaddr)) {
		err("invaid ip address: %s", ipaddr);
		exit(EXIT_FAILURE);
	}

	/* process netmask */
	if (!inet_aton(netmask, &lan_netmask)) {
		err("invalid netmask: %s", netmask);
		exit(EXIT_FAILURE);
	}

	lan_network.s_addr = (lan_ipaddr.s_addr & lan_netmask.s_addr);
}

static int plugin_init(fd_set * rfds, int *nfds)
{
	extern struct plugin neigh_plugin;
	extern struct plugin plc_plugin;
	extern struct plugin ssdp_plugin;

	struct plugin *plugins[] = {
		&neigh_plugin,
		&plc_plugin,
		&ssdp_plugin,
		NULL,
	};
	struct plugin **p;

	FD_ZERO(rfds);
	*nfds = 0;

	for (p = plugins; *p; ++p) {
		struct plugin *plugin = *p;

		if ((plugin->init(plugin) < 0)) {
			err("%s init failed", plugin->name);
			return -1;
		}

		list_add_tail(&plugin->list, &plugin_list);
		FD_SET(plugin->fd, rfds);
		*nfds = max(*nfds, plugin->fd);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct plugin *plugin;
	struct timespec ts;
	struct timespec to;
	fd_set rfds;
	unsigned long poll_date;
	int nfds;

	memset(&to, 0x00, sizeof(to));
	setup(argc, argv);

	if (plugin_init(&rfds, &nfds) < 0) {
		exit(EXIT_FAILURE);
	}

	dns_setup_local();
	clock_gettime(CLOCK_MONOTONIC, &ts);
	now = ts.tv_sec;
	poll_date = now + POLL_INTERVAL;

	for (;;) {
		fd_set _rfds = rfds;
		int _nfds = nfds + 1;

		to.tv_sec = poll_date - now;
		if ((_nfds =
		     linux_pselect(_nfds, &_rfds, NULL, NULL, &to, NULL)) < 0) {
			if (errno == EINTR)
				continue;

			sys_err("%s()", "linux_pselect");
			break;
		}

		clock_gettime(CLOCK_MONOTONIC, &ts);
		now = ts.tv_sec;
		if (time_after_eq(now, poll_date)) {
			poll_date = now + POLL_INTERVAL;
			list_for_each_entry(plugin, &plugin_list, list) {
				if (plugin->poll)
					(plugin->poll) (plugin);
			}
		}

		list_for_each_entry(plugin, &plugin_list, list) {
			if (FD_ISSET(plugin->fd, &_rfds))
				(plugin->event) (plugin);
		}
	}

	list_for_each_entry(plugin, &plugin_list, list) {
		(plugin->cleanup) (plugin);
	}

	return 0;
}
