
#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <etk/log.h>
#include <etk/string.h>

#include <nbd/core.h>
#include <nbd/lan.h>
#include <nbd/stb.h>

#include "plugin.h"

#define SSDP_PORT 1900
#define SSDP_MCAST_ADDR "239.255.255.250"

static int ssdp_init(struct plugin *plugin);
static void ssdp_cleanup(struct plugin *plugin);
static int ssdp_event(struct plugin *plugin);

struct plugin ssdp_plugin = {
	.name = "ssdp",
	.init = ssdp_init,
	.cleanup = ssdp_cleanup,
	.event = ssdp_event,
};

extern char lan_ifname[16];

static int ssdp_notify_event(struct plugin *plugin, char *buf, ssize_t n,
			     struct sockaddr_in *in)
{
	char buf1[16];
	char buf2[64];
	char const *name;
	char const *value;
	char *nt = NULL;
	char *nts = NULL;
	char const *ipaddr;
	char macaddr[32];

	/* NOTIFY packet. */
	dbg("SSDP: NOTIFY event");

	/* dissect pkt */
	for_each_line(line, buf) {
		dbg("line = %s", line);

		if (sscanf(line, "%15[^:]:%127[^\r\n]", buf1, buf2) != 2)
			continue;

		name = strtrim(buf1);
		value = strtrim(buf2);

		dbg("SSDP: NOTIFY event: %s=%s", name, value);

		if ((!nt) && (!strcasecmp(name, "nt")))
			nt = strdup(value);
		else if ((!nts) && (!strcasecmp(name, "nts")))
			nts = strdup(value);

		if ((! !nt) && (! !nts))
			break;
	}

	/* have all needed values ? */
	if ((!nt) || (!nts)) {
		err("SSDP NOTIFY event: broken packet: nt:%s nts:%s", nt, nts);
		goto out;
	}

	dbg("SSDP NOTIFY event: nt:%s nts:%s", nt, nts);

	if (matches(nt, "urn:neufboxtv-org:device:SetTopBox:1")
	    && matches(nts, "ssdp:alive")) {
		/* We have a STB which announce this presence */
		dbg("ssdp:alive NOTIFY pkt");

		ipaddr = inet_ntoa(in->sin_addr);

		nbd_open();
		/* get macaddr */
		if (nbd_lan_ip2mac(ipaddr, macaddr, sizeof(macaddr)) < 0) {
			info("%s(%s)", "nbd_lan_ip2mac", ipaddr);
			nbd_close();
			goto out;
		}

		/* always add stb, so that topology can update counters */
		info("STB(%s) probed at %s", macaddr, ipaddr);
		nbd_stb_add(macaddr, ipaddr, "");
		nbd_close();
	}

 out:
	free(nt);
	free(nts);
	return 0;
}

static int ssdp_event(struct plugin *plugin)
{
	struct sockaddr_in in;
	socklen_t len = sizeof(in);
	char buf[1500];
	ssize_t n;

	n = recvfrom(plugin->fd, buf, sizeof(buf), 0, (void *)&in, &len);
	if (n < 0) {
		sys_err("%s(%d)", "recvfrom", plugin->fd);
		return -1;
	}

	if (!memcmp(buf, "NOTIFY", sizeof("NOTIFY") - 1))
		return ssdp_notify_event(plugin, buf, n, &in);

	return 0;
}

static int ifaddr(char const *ifname, struct in_addr *addr)
{
	/* SIOCGIFADDR struct ifreq *  */
	struct ifreq ifr;
	int len;
	int fd;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		sys_err("%s(%s, %s, 0)", "socket", "PF_INET", "SOCK_DGRAM");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	/* IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", ifname);
	len = sizeof(ifr);
	if (ioctl(fd, SIOCGIFADDR, &ifr, &len) < 0) {
		sys_err("%s(%d, %s, %s)", "ioctl", fd, "SIOCGIFADDR", ifname);
		close(fd);
		return -1;
	}

	*addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;

	close(fd);

	return 0;
}

static int ssdp_init(struct plugin *plugin)
{
	struct sockaddr_in in;
	struct ip_mreq imr;	/* Ip multicast membership */
	struct in_addr in_addr;
	int i;

	/* get ifaddr */
	if (ifaddr(lan_ifname, &in_addr) != 0) {
		err("getifaddr(%s)", lan_ifname);
		return -1;
	}

	/*************** setup plugin->fd ***************/
	if ((plugin->fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		sys_err("%s(%s, %s, 0)", "socket", "PF_INET", "SOCK_DGRAM");
		return -1;
	}

	i = 1;
	if (setsockopt(plugin->fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0) {
		sys_err("%s(%d, %s)", "setsockopt", plugin->fd, "SO_REUSEADDR");
		close(plugin->fd);
		return -1;
	}

	memset(&in, 0, sizeof(in));
	in.sin_family = AF_INET;
	in.sin_port = htons(SSDP_PORT);
	in.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(plugin->fd, (void *)&in, sizeof(in)) < 0) {
		sys_err("%s(%d)", "bind", plugin->fd);
		close(plugin->fd);
		return -1;

	}

	/* setting up imr structure */
	imr.imr_multiaddr.s_addr = inet_addr(SSDP_MCAST_ADDR);
	imr.imr_interface.s_addr = in_addr.s_addr;

	if (setsockopt
	    (plugin->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&imr,
	     sizeof(imr)) < 0) {
		sys_err("%s(%d, %s, %s)", "setsockopt", plugin->fd,
			"IPPROTO_IP", "IP_ADD_MEMBERSHIP");
		close(plugin->fd);
		return -1;
	}

	return 0;
}

static void ssdp_cleanup(struct plugin *plugin)
{
	close(plugin->fd);
}
