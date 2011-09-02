#ifndef _NBL_NET_H_
#define _NBL_NET_H_

#include <stdlib.h>
#include <string.h>

#include "nbu/nbu.h"
#include "nbu/list.h"

typedef struct nbl_net_lan_client {
	char *mac;
	char *ip;
	char *port;
} nbl_net_lan_client_t;

enum nbl_net_getipsrc_err_t {
        NBL_NET_GETIPSRC_ERR_DNS_TMPFAIL = -1000,
        NBL_NET_GETIPSRC_ERR_DNS_UNKNOWNDEST,
        NBL_NET_GETIPSRC_ERR_UNKNOWN = NBU_ERROR,
        NBL_NET_GETIPSRC_SUCCESS = NBU_SUCCESS,
};

/*
 * get ip address from mac address
 */
extern int nbl_net_ip_from_mac(const char *macaddr, char *ip, size_t ip_size);

/*
 * get mac address from ip address
 */
extern int nbl_net_mac_from_ip(const char *ip, char *mac, size_t mac_size);

/*
 * get client mac address of the webui
 */
extern int nbl_net_client_mac(char *mac, size_t mac_size);

/*
 * get client hostname of the webui
 */
extern int nbl_net_client_hostname(char *hostname, size_t hostname_size);

/*
 * Get the ip src to bind (used in ping/traceroute program) from ip_dest
 */
extern int nbl_net_get_ipsrc_from_ipdest(const char *host_dest, char *ip_source, size_t len);

#endif /* #define _NBL_NET_H_ */
