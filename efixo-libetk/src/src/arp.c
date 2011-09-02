
#include <string.h>
#include <stdio.h>

#include <linux/if_bridge.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <etk/log.h>
#include <etk/exec.h>

#define BUFFER_IP_SIZE sizeof("xxx.xxx.xxx.xxx")
#define BUFFER_MAC_SIZE sizeof("xx:xx:xx:xx:xx:xx")

static int __arp_mac_from_ip(char const *ifname, char const *ip_addr,
			     char *mac_addr, size_t len, int use_arping)
{
	FILE *fp;
	char buf[128];

	if (len < BUFFER_MAC_SIZE) {
		err("Buffer must be larger than %zu", BUFFER_MAC_SIZE);
		return -1;
	}

	/* refresh ARP table */
	if (use_arping) {
		exec("arping", "-I", ifname, "-c", "1", "-q", "-w", "1",
		     ip_addr);
	}

	fp = fopen("/proc/net/arp", "r");
	if (!fp) {
		return -1;
	}

	/* skip first line */
	if (fgets(buf, sizeof(buf), fp) == NULL)
		return -1;

	while (!feof(fp)) {
		unsigned int flags;

		if (fscanf(fp, " %15[0-9.] %*x %x %17[A-F,a-f,0-9:] %*s %*s",
			   buf, &flags, mac_addr) < 3) {
			continue;
		}

		/* completed entry (ha valid) */
		if ((flags & ATF_COM) && (!strcmp(ip_addr, buf))) {
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);

	if (use_arping) {
		err("%s %s failed", ifname, ip_addr);
	}

	return -1;
}

int arp_mac_from_ip(char const *ifname, char const *ip_addr, char *mac_addr,
		    size_t len)
{
	return !__arp_mac_from_ip(ifname, ip_addr, mac_addr, len, 0) ?
	    0 : __arp_mac_from_ip(ifname, ip_addr, mac_addr, len, 1);
}

int arp_ip_from_mac(char const *ifname, char const *mac_addr, char *ip_addr,
		    size_t len)
{
	FILE *fp = NULL;
	char buf[128];

	if (len < BUFFER_IP_SIZE) {
		err("Buffer must be larger than %zu", BUFFER_IP_SIZE);
		return -1;
	}

	fp = fopen("/proc/net/arp", "r");
	if (fp == NULL) {
		return -1;
	}

	/* skip first line */
	if (fgets(buf, sizeof(buf), fp) == NULL)
		return -1;

	while (!feof(fp)) {
		unsigned int flags;

		if (fscanf(fp, " %15[0-9.] %*x %x %17[A-F,a-f,0-9:] %*s %*s",
			   ip_addr, &flags, buf) < 3) {
			continue;
		}

		if ((flags & ATF_COM) && (!strcasecmp(mac_addr, buf))) {
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);

	err("%s %s failed", ifname, mac_addr);

	return -1;
}

int __arp_add_del_entry(char const *ifname, char const *ip_addr,
			char const *mac_addr, int add)
{
	exec("ip", "neigh", add ? "replace" : "del", ip_addr,
	     "lladdr", mac_addr, "dev", ifname, "nud", "permanent");

	note("ARP: %s %s %s %s", add ? "add" : "del", ifname, ip_addr,
	     mac_addr);

	return 0;
}
