#include "nbl.h"
#include "nbl_net.h"

#include "nbd.h"

#include <errno.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if_arp.h>

#include <nbu/nbu.h>
#include <nbu/string.h>

#include <nbd/config.h>

#include <ewf/ewf_utils.h>

int nbl_net_ip_from_mac(const char *macaddr, char *ip, size_t ip_size)
{
	FILE *proc = NULL;
	char proc_ipaddr[15 + 1];
	char proc_macaddr[17 + 1];
	char proc_flags[4 + 1];
	int flags;
	int ret = NBU_ERROR;

	proc = fopen("/proc/net/arp", "r");
	if (proc == NULL)
	{
		return ret;
	}

	memset(proc_ipaddr, 0, sizeof proc_ipaddr);
	memset(proc_macaddr, 0, sizeof proc_macaddr);
	memset(proc_flags, 0, sizeof proc_flags);

	/* skip first line */
	while (!feof(proc) && fgetc(proc) != '\n');

	while (!feof(proc)
	       && (fscanf(proc, " %15[0-9.] %*s %4[0-9x] %17[A-F0-9:] %*s %*s",
			  proc_ipaddr, proc_flags, proc_macaddr) == 3))
	{
		errno = 0;
		flags = strtol(proc_flags, NULL, 16);

		if(errno != 0)
		{
			/* invalid flags ! */
			fprintf(stderr, "Invalid flags in /proc/net/arp !\n");
			continue;
		}

		if (nbu_string_compare(proc_macaddr, macaddr) == NBU_STRING_EQUAL
		    && flags == ATF_COM)
		{
			/* ATF_COM            completed entry (ha valid) */
			nbu_string_printf(ip, ip_size, "%s", proc_ipaddr);
			ret = NBU_SUCCESS;
			break;
		}
	}

	fclose(proc);

	return ret;
}

int nbl_net_mac_from_ip(const char *ip, char *mac, size_t mac_size)
{
	FILE *proc;
	char proc_ip[15 + 1];
	char proc_mac[17 + 1];
	char proc_flags[4 + 1];
	int flags;
	int ret = NBU_ERROR;

	if(ip == NULL)
	{
		return NBU_ERROR;
	}

	proc = fopen("/proc/net/arp", "r");

	if (proc != NULL)
	{
		memset(proc_ip, 0, sizeof proc_ip);
		memset(proc_mac, 0, sizeof proc_mac);
		memset(proc_flags, 0, sizeof proc_flags);

		/* skip first line */
		while (!feof(proc) && fgetc(proc) != '\n');

		while (!feof(proc)
		       && (fscanf(proc, " %15[0-9.] %*s %4[0-9x] %17[A-F0-9:] %*s %*s",
				  proc_ip, proc_flags, proc_mac) == 3))
		{
			errno = 0;
			flags = strtol(proc_flags, NULL, 16);

			if(errno != 0)
			{
				/* invalid flags ! */
				fprintf(stderr, "Invalid flags in /proc/net/arp !\n");
				continue;
			}

			if (nbu_string_compare(proc_ip, ip) == NBU_SUCCESS
			    && flags == ATF_COM)
			{
				/* ATF_COM            completed entry (ha valid) */
				nbu_string_printf(mac, mac_size, "%s", proc_mac);
				ret = NBU_SUCCESS;

				break;
			}
		}

		fclose(proc);
	}
	return ret;
}

int nbl_net_client_mac(char *mac, size_t mac_size)
{
	return nbl_net_mac_from_ip(ewf_utils_client_ip(), mac, mac_size);
}

int nbl_net_hostname_from_ip(const char *ip, char *hostname, size_t hostname_size)
{
	FILE *leases;
	char leases_ip[15 + 1];
	char leases_hostname[64 + 1];
	int ret = NBU_ERROR;

	leases = fopen("/var/dnsmasq.leases", "r");

	if (leases != NULL)
	{
		memset(leases_ip, 0, sizeof leases_ip);
		memset(leases_hostname, 0, sizeof leases_hostname);

		/* skip first line */
		while (!feof(leases) && fgetc(leases) != '\n');

		while (!feof(leases)
		   && (fscanf(leases, " %*s %*s %15[0-9.] %64s %*s", leases_ip, leases_hostname) == 2))
		{
			if (nbu_string_compare(leases_ip, ip) == NBU_SUCCESS)
			{
				if(nbu_string_compare(leases_hostname, "*") != NBU_SUCCESS)
				{
					ret = NBU_SUCCESS;
				}

				snprintf(hostname, hostname_size, "%s", leases_hostname);

				break;
			}
		}

		fclose(leases);
	}

	return ret;
}

int nbl_net_get_ipsrc_from_ipdest(const char *host_dest,
                                  char *buf,
                                  size_t buf_size)
{
	char lan_ip[16];
	char lan_netmask[16];
	char ipaddr_data[16];
        char ipaddr_dest[16];

	struct in_addr lan_ip_addr;
	struct in_addr lan_netmask_addr;
	struct in_addr ip_dest_addr;
	unsigned long int lan_network_ip;

        struct addrinfo hints;
        struct addrinfo *res = NULL;
        int e;
        char serv[64];

	/* Process: Get lan/netmask and check if host_dest is in the inside
         *          network
	 *    -> if ip_dest is in the inside network, ipsrc is 0.0.0.0
	 *    -> otherwise ipsrc is "data ip"
	 */

	if(buf_size < 16)
	{
		nbu_log_error("Need more space to stock ip address !"
                              " %u < 16", buf_size);
		return NBL_NET_GETIPSRC_ERR_UNKNOWN;
	}

	/* get lan ip */
	if (nbd_user_config_get("lan_ipaddr",
                       lan_ip, sizeof(lan_ip)) != NBD_SUCCESS)
	{
		return NBL_NET_GETIPSRC_ERR_UNKNOWN;
	}

	/* get lan netmask */
	if (nbd_user_config_get("lan_netmask",
                       lan_netmask, sizeof(lan_netmask)) != NBD_SUCCESS)
	{
		return NBL_NET_GETIPSRC_ERR_UNKNOWN;
	}

	/* get ip ppp */
	if(nbd_status_get("net_data_ipaddr",
                          ipaddr_data, sizeof(ipaddr_data)) != NBD_SUCCESS)
	{
		return NBL_NET_GETIPSRC_ERR_UNKNOWN;
	}

	/* get ip of host_dest */
        memset(&hints, '\0', sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_ADDRCONFIG;

        e = getaddrinfo(host_dest, "http", &hints, &res);
        if(e != 0)
        {
                nbu_log_error("getaddrinfo(%s) failed: %s",
                              host_dest, gai_strerror(e));
                if(e == EAI_AGAIN)
                {
                        return NBL_NET_GETIPSRC_ERR_DNS_TMPFAIL;
                }
                else if(e == EAI_NONAME)
                {
                        return NBL_NET_GETIPSRC_ERR_DNS_UNKNOWNDEST;
                }
                else
                {
                        return NBL_NET_GETIPSRC_ERR_UNKNOWN;
                }
        }

        if(res == NULL)
        {
                nbu_log_error("Unable to find ipaddr for '%s'",
                              host_dest);
                return NBL_NET_GETIPSRC_ERR_UNKNOWN;
        }

        getnameinfo((const struct sockaddr *)res->ai_addr,
                    res->ai_addrlen,  ipaddr_dest, sizeof(ipaddr_dest),
                    serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);

        freeaddrinfo(res);

	/* Calculate network address */
	if ( inet_aton( lan_ip, &lan_ip_addr ) == 0
	     || inet_aton( lan_netmask, &lan_netmask_addr ) == 0
             || inet_aton( ipaddr_dest, &ip_dest_addr ) == 0)
	{
                nbu_log_error("%m");
		return NBL_NET_GETIPSRC_ERR_UNKNOWN;
	}

	lan_network_ip = lan_ip_addr.s_addr & lan_netmask_addr.s_addr;

	if((ip_dest_addr.s_addr & lan_netmask_addr.s_addr) == lan_network_ip
	   || ipaddr_data[0] == '\0')
	{
		/* ip_dest is in the lan OR wan is down */
		nbu_string_copy(buf, buf_size, "0.0.0.0");
	} else {
		nbu_string_copy(buf, buf_size, ipaddr_data);
	}

	return NBL_NET_GETIPSRC_SUCCESS;
}
