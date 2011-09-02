
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <linux/if_bridge.h>

#include <etk/log.h>
#include <etk/io.h>

int bridge_if_indextoname(char const *bridge, unsigned port_no, char *ifname)
{
	int fd;
	struct ifreq ifr;
	unsigned ifindices[256];
	unsigned long args[4] = {
		BRCTL_GET_PORT_LIST,
		(unsigned long)ifindices,
		sizeof(ifindices) / sizeof(int),
		0
	};

	memset(ifindices, 0x00, sizeof ifindices);
	strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
	ifr.ifr_data = (char *)&args;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		sys_err("%s(%u, %u, %u)", "socket", AF_INET, SOCK_DGRAM, 0);

		return -1;
	}

	if (ioctl(fd, SIOCDEVPRIVATE, &ifr) < 0) {
		sys_err("%s(%d, %u, %p)", "ioctl", fd, SIOCDEVPRIVATE, &ifr);
		close(fd);

		return -1;
	}

	close(fd);

	if (if_indextoname(ifindices[port_no], ifname) == NULL) {
		sys_err("%s(%d, %s)", "if_indextoname",
			ifindices[port_no], ifname);

		return -1;
	}

	return 0;
}

int net_bridge_port(char const *bridge, struct ether_addr *e, char *buf,
		    size_t len)
{
	char file[sizeof("/sys/class/net/xxxxxxxxxxxxxxxx/brforward")];
	struct __fdb_entry fdbs[64];
	int n;
	int i;

	snprintf(file, sizeof(file), "/sys/class/net/%s/brforward", bridge);
	n = file_open_read_close(file, &fdbs, sizeof(fdbs));
	if (n < 0) {
		err("%s(%s, %p, %zu)", "file_open_read_close",
		    file, &fdbs, sizeof(fdbs));
		return -1;
	}

	n /= sizeof(struct __fdb_entry);
	for (i = 0; i < n; ++i) {
		struct __fdb_entry *fdb = &fdbs[i];
		char ifname[IFNAMSIZ];

		dbg("%02x:%02x:%02x:%02x:%02x:%02x "
		    "?= %02x:%02x:%02x:%02x:%02x:%02x",
		    fdb->mac_addr[0], fdb->mac_addr[1],
		    fdb->mac_addr[2], fdb->mac_addr[3],
		    fdb->mac_addr[4], fdb->mac_addr[5],
		    e->ether_addr_octet[0], e->ether_addr_octet[1],
		    e->ether_addr_octet[2], e->ether_addr_octet[3],
		    e->ether_addr_octet[4], e->ether_addr_octet[5]);

		if (fdb->is_local
		    || memcmp(fdb->mac_addr, e->ether_addr_octet, 6) != 0) {
			continue;
		}

		memset(ifname, 0x00, sizeof(ifname));
		if (bridge_if_indextoname
		    (bridge, (unsigned)fdb->port_no, ifname) < 0) {
			err("%s(%s, %d, %p)", "bridge_if_indextoname",
			    bridge, fdb->port_no, ifname);
			return -1;
		}

		snprintf(buf, len, "%s", ifname);

		return 0;
	}

	return -1;
}
