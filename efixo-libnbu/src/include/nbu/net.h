#ifndef _NBU_NET_H
#define _NBU_NET_H

#include <string.h>

#define NBU_NET_SIZEOF_IPADDR 16
#define NBU_NET_SIZEOF_MACADDR 18

enum nbu_net_errors {
	nbu_net_err_invalid_ip = -1000,
	nbu_net_err_invalid_mask,
	nbu_net_err_ip_is_network,
	nbu_net_err_ip_is_broadcast,
};

extern int nbu_net_validate_netmask(const char *netmask);
extern int nbu_net_validate_ip(const char *ip);
extern int nbu_net_validate_full_ip(const char *ip, const char *netmask);
extern int nbu_net_validate_mac(const char *mac);

extern int nbu_net_split_ip(const char *ip, char parts[4][4]);
extern int nbu_net_join_ip(const char *p0, const char *p1, const char *p2, const char *p3, 
			   char *ip, size_t len_ip);

extern int nbu_net_split_mac(const char *mac, char parts[6][3]);
extern int nbu_net_join_mac(const char *p0, const char *p1, const char *p2, const char *p3, const char *p4, const char *p5, 
			    char *mac, size_t len_mac);

extern int nbu_net_split_hostname(const char *hostname, 
				  char *host, size_t host_len, 
				  char *domain, size_t domain_len);
extern int nbu_net_join_hostname(const char *host, const char *domain, 
				 char *hostname, size_t hostname_len);

/*
 * get network address (ip/small netmask) with ip and long netmask
 */
extern int nbu_net_get_network(const char *ip, const char *netmask,
			       char *network, size_t network_size);

/*
 * ip full representation to simplified representation
 */
extern int nbu_net_ipfr2ipsr(const char *ip, const char *netmask,
			     char *ezip, size_t ezip_size);

/*
 * get hostname with an ip
 */
extern int nbu_net_gethostname(const char *ip, char *hbuf, size_t hbuf_size);

#endif
