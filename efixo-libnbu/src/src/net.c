#include "nbu/net.h"

#include "nbu/nbu.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>

int nbu_net_validate_netmask(const char *netmask)
{
	char parts[4][4];
	unsigned int nm;
	int n, ret = NBU_ERROR;
	long i;

	if(netmask == NULL)
	{
		return ret;
	}
	
	if (nbu_net_split_ip(netmask, parts) == 0)
	{
		ret = NBU_SUCCESS;
		nm = 0;

		for (n = 0; n < 4; n++)
		{
			errno = 0;

			i = strtol(parts[n], NULL, 10);

			if ((errno == ERANGE && (i == LONG_MAX || i == LONG_MIN))
			    || (errno != 0 && i == 0))
			{
				ret = NBU_ERROR;
				break;
			}

			nm += (i << ((3 - n) * 8));
		}

		if (ret == NBU_SUCCESS)
		{
			ret = NBU_ERROR;

			/* We want a binary number containing '1' bits followed
			 * by '0' bits.
			 *
			 * Take the mask and AND it with the twos complement of the
			 * mask, resulting in the rightmost '1' in the mask. The
			 * twos complement of such rightmost '1' should equal the
			 * original mask.
			 */
			if (-(nm & -nm) == nm)
			{
				ret = NBU_SUCCESS;
			}
		}
	}

	return ret;
}

int nbu_net_validate_ip(const char *ip)
{
	int parts[4];
	char buffer[17];
	size_t n;
	int ret = NBU_ERROR;

	if(ip == NULL)
	{
		return ret;
	}
	
	nbu_string_copy(buffer, sizeof buffer, ip);
	n = strlen(buffer);

	/* minimum length is 7 and maximum is 15 */
	if ((n > 6) && (n < 16))
	{
		/* check that it contains only allowed characters */
		if (strspn(buffer, "0123456789.") == n)
		{
			if (sscanf(buffer, "%3d.%3d.%3d.%3d", &parts[0], &parts[1], &parts[2], &parts[3]) == 4)
			{
				if (((parts[0] | parts[1] | parts[2] | parts[3]) & 0xffffff00) == 0)
				{
					ret = NBU_SUCCESS;
				}
			}
		}
	}

	return ret;
}

int nbu_net_validate_full_ip(const char *ip, const char *netmask)
{
	in_addr_t in_ip_addr, in_mask_addr, in_mask_wildcard_addr,
		in_network_addr;
	
	/* quick validation */
	if(nbu_net_validate_ip(ip) != NBU_SUCCESS)
	{
		return nbu_net_err_invalid_ip;
	}
	
	if(nbu_net_validate_netmask(netmask) != NBU_SUCCESS)
	{
		return nbu_net_err_invalid_mask;
	}

	/* transform ascii to workeable type (in network order) */
	if((in_ip_addr = inet_addr(ip)) == (in_addr_t)(-1))
	{
		return nbu_net_err_invalid_ip;
	}

	if((in_mask_addr = inet_addr(netmask)) == (in_addr_t)(-1))
	{
		return nbu_net_err_invalid_mask;
	}
	
	in_mask_wildcard_addr = ~(in_mask_addr);
	in_network_addr = (in_ip_addr & in_mask_addr);
	
	/* check if ip doesn't equal to network ip */
	if(in_network_addr == in_ip_addr)
	{
		return nbu_net_err_ip_is_network;
	}
	
	/* check if ip doesn't equal to broadcast ip */
	if((in_network_addr | in_mask_wildcard_addr) == in_ip_addr)
	{
		return nbu_net_err_ip_is_broadcast;
	}

	return NBU_SUCCESS;
}

int nbu_net_validate_mac(const char *mac)
{
	int parts[6];
	char buffer[18];
	size_t n;
	int ret = NBU_ERROR;

	if(mac == NULL)
	{
		return ret;
	}
	
	nbu_string_copy(buffer, sizeof buffer, mac);
	n = strlen(buffer);

	/* length must be 17 */
	if (n == 17)
	{
		/* check that it contains only allowed characters */
		if (sscanf(buffer, "%2x:%2x:%2x:%2x:%2x:%2x", 
			   &parts[0], &parts[1], &parts[2], 
			   &parts[3], &parts[4], &parts[5]) == 6)
		{
			ret = NBU_SUCCESS;
		}
	}
	
	return ret;
}

int nbu_net_split_ip(const char *ip, char parts[4][4])
{
	int iparts[4];
	char buffer[17];
	size_t n;
	int ret = NBU_ERROR;

	if(ip == NULL)
	{
		return ret;
	}
	
	nbu_string_copy(buffer, sizeof buffer, ip);
	n = strlen(buffer);

	/* minimum length is 7 and maximum is 15 */
	if ((n > 6) && (n < 16))
	{
		/* check that it contains only allowed characters */
		if (strspn(buffer, "0123456789.") == n)
		{
			if (sscanf(buffer, "%3d.%3d.%3d.%3d", &iparts[0], &iparts[1], &iparts[2], &iparts[3]) == 4)
			{
				if (((iparts[0] | iparts[1] | iparts[2] | iparts[3]) & 0xffffff00) == 0)
				{
					for (n = 0; n < 4; n++)
					{
						memset(parts[n], '\0', 4);
						nbu_string_printf(parts[n], sizeof parts[n], "%d", iparts[n]);
					}

					ret = NBU_SUCCESS;
				}
			}
		}
	}

	return ret;
}

int nbu_net_split_mac(const char *mac, char parts[6][3])
{
	unsigned int mparts[6];
	char buffer[19];
	size_t n;
	int ret = NBU_ERROR;

	if(mac == NULL)
	{
		return ret;
	}
	
	nbu_string_copy(buffer, sizeof buffer, mac);
	n = strlen(buffer);

	/* minimum length is 11 and maximum is 17 */
	if ((n > 10) && (n < 18))
	{
		/* check that it contains only allowed characters */
		if (strspn(buffer, "0123456789abcdefABCDEF:") == n)
		{
			if (sscanf(buffer, "%2x:%2x:%2x:%2x:%2x:%2x", &mparts[0], &mparts[1], &mparts[2], &mparts[3], &mparts[4], &mparts[5]) == 6)
			{
				if (((mparts[0] | mparts[1] | mparts[2] | mparts[3] | mparts[4] | mparts[5]) & 0xffffff00) == 0)
				{
					for (n = 0; n < 6; n++)
					{
						memset(parts[n], '\0', 3);
						nbu_string_printf(parts[n], sizeof parts[n], "%02X", mparts[n]);
					}

					ret = NBU_SUCCESS;
				}
			}
		}
	}

	return ret;
}

/* returns -1 on error or if hostname is not valid */
int nbu_net_split_hostname(const char *hostname, char *host, size_t host_len, char *domain, size_t domain_len)
{
	char *c = NULL;
	int ret = NBU_ERROR;

	if(hostname == NULL || host == NULL || domain == NULL)
	{
		return ret;
	}
	
	c = strchr(hostname, '.');

	if ((c != NULL) && (*(c + 1) != '\0'))
	{
		/* strn-cpy(host, hostname, c - hostname); */
		nbu_string_printf(host, host_len, "%.*s", c - hostname, hostname);

		/* strn-cpy(domain, c + 1, NBD_BUFFER_SIZE - 1); */
		nbu_string_copy(domain, domain_len, c + 1);

		ret = NBU_SUCCESS;
	}

	return ret;
}

/* returns -1 on error or if hostname is not valid */
int nbu_net_join_hostname(const char *host, const char *domain, char *hostname, size_t hostname_len)
{
	/* 0 < length(hostname) < 21, only letters and digits */
	int n = 0;
	int ret = NBU_ERROR;

	if(host == NULL || domain == NULL)
	{
		return ret;
	}

	if ((host != NULL) && (domain != NULL))
	{
		n = strlen(host);
		if ((n > 0) && (n < 21))
		{
			nbu_string_printf(hostname, hostname_len, "%s.%s", host, domain);

			ret = NBU_SUCCESS;
		}
	}

	return ret;
}

/* returns -1 on error or if IP is not valid */
int nbu_net_join_ip(const char *p0, const char *p1, const char *p2, const char *p3, char *ip, size_t ip_len)
{
	const char *p[4];
	char *endptr;
	int parts[4];
	int n, m, ret = NBU_ERROR;

	if ((p0 != NULL) && (p1 != NULL) && (p2 != NULL) && (p3 != NULL))
	{
		p[0] = p0;
		p[1] = p1;
		p[2] = p2;
		p[3] = p3;

		ret = NBU_SUCCESS;

		for (n = 0; n < 4; n++)
		{
			/* strn-cpy(ip, p[n], 16); */
			nbu_string_copy(ip, ip_len, p[n]);
			m = strlen(ip);

			if ((m < 1) || (m > 3))
			{
				ret = NBU_ERROR;
				break;
			}

			errno = 0;

			parts[n] = strtol(ip, &endptr, 10);

			if ((endptr == ip) || (*endptr != '\0')
			 || (errno == ERANGE && (parts[n] == LONG_MAX || parts[n] == LONG_MIN))
			 || (errno != 0 && parts[n] == 0))
			{
				ret = -1;
				break;
			}

		}

		if (ret == NBU_SUCCESS)
		{
			ret = NBU_ERROR;

			if (((parts[0] | parts[1] | parts[2] | parts[3]) & 0xffffff00) == 0)
			{
				nbu_string_printf(ip, ip_len, "%d.%d.%d.%d", parts[0], parts[1], parts[2], parts[3]);

				ret = NBU_SUCCESS;
			}
		}
	}

	return ret;
}

/* returns -1 on error or if MAC is not valid */
int nbu_net_join_mac(const char *p0, const char *p1, const char *p2, const char *p3, const char *p4, const char *p5, char *mac, size_t mac_len)
{
	const char *p[6];
	char *endptr;
	int parts[6];
	int n, m, ret = NBU_ERROR;

	if ((p0 != NULL) && (p1 != NULL) && (p2 != NULL) && (p3 != NULL) && (p4 != NULL) && (p5 != NULL))
	{
		p[0] = p0;
		p[1] = p1;
		p[2] = p2;
		p[3] = p3;
		p[4] = p4;
		p[5] = p5;

		ret = NBU_SUCCESS;

		for (n = 0; n < 6; n++)
		{
			/* strn-cpy(mac, p[n], 18); */
			nbu_string_copy(mac, mac_len, p[n]);
			m = strlen(mac);

			if ((m < 1) || (m > 2))
			{
				ret = NBU_ERROR;
				break;
			}

			errno = 0;

			parts[n] = strtol(mac, &endptr, 16);

			if ((endptr == mac) || (*endptr != '\0')
			 || (errno == ERANGE && (parts[n] == LONG_MAX || parts[n] == LONG_MIN))
			 || (errno != 0 && parts[n] == 0))
			{
				ret = NBU_ERROR;
				break;
			}

		}

		if (ret == NBU_SUCCESS)
		{
			ret = NBU_ERROR;

			if (((parts[0] | parts[1] | parts[2] | parts[3] | parts[4] | parts[5]) & 0xffffff00) == 0)
			{
				nbu_string_printf(mac, mac_len, "%02X:%02X:%02X:%02X:%02X:%02X", parts[0], parts[1], parts[2], parts[3], parts[4], parts[5]);

				ret = NBU_SUCCESS;
			}
		}
	}

	return ret;
}

int nbu_net_get_network(const char *ip, const char *netmask,
			char *network, size_t network_size)
{
	struct in_addr in_ipaddr;
	struct in_addr in_netmask;
	struct in_addr in_network;
	unsigned int mask = 32U;

	/* process IP address */
	if ( !inet_aton( ip, &in_ipaddr ) )
	{
		return NBU_ERROR;
	}

	/* process netmask */
	if ( !inet_aton( netmask, &in_netmask ) ) 
	{
		return NBU_ERROR;
	}

	/* Valid IP address and netmask */
	in_ipaddr.s_addr = ntohl( in_ipaddr.s_addr );
	in_netmask.s_addr = ntohl( in_netmask.s_addr );

	mask -= ffs( in_netmask.s_addr ) - 1U;
	if ( mask > 32U ) { /* maybe someone can set netmask to 0.0.0.0 ? */
		mask = 0U;
	}

	in_network.s_addr = htonl( in_ipaddr.s_addr & in_netmask.s_addr );

	snprintf(network, network_size, "%s/%u\n", inet_ntoa( in_network ), mask );

	return NBU_SUCCESS;
}

int nbu_net_ipfr2ipsr(const char *ip, const char *netmask,
		      char *ezip, size_t ezip_size)
{
	struct in_addr in_ipaddr;
	struct in_addr in_netmask;
	unsigned int mask = 32U;

	/* process IP address */
	if ( !inet_aton( ip, &in_ipaddr ) )
	{
		return NBU_ERROR;
	}

	/* process netmask */
	if ( !inet_aton( netmask, &in_netmask ) ) 
	{
		return NBU_ERROR;
	}

	/* Valid IP address and netmask */
	in_ipaddr.s_addr = ntohl( in_ipaddr.s_addr );
	in_netmask.s_addr = ntohl( in_netmask.s_addr );

	mask -= ffs( in_netmask.s_addr ) - 1U;
	if ( mask > 32U ) { /* maybe someone can set netmask to 0.0.0.0 ? */
		mask = 0U;
	}

	snprintf(ezip, ezip_size, "%s/%u\n", ip, mask );

	return NBU_SUCCESS;
}

int nbu_net_gethostname(const char *ip, char *hbuf, size_t hbuf_size)
{
	int ret = NBU_ERROR;
	struct sockaddr_in sa_in;
	
	hbuf[0] = '\0';
	do
	{
		/* fill sockaddr_in struct */
		memset((void*)&sa_in, 0, sizeof(struct sockaddr_in));
		sa_in.sin_family = AF_INET;
		if(inet_aton(ip, &(sa_in.sin_addr)) == 0)
		{
			/* failed */
			break;
		}
	
		/* resolv */
		if(getnameinfo((const struct sockaddr*)&sa_in, sizeof(struct sockaddr_in), 
			       hbuf, hbuf_size,
			       NULL, 0, 
			       NI_NAMEREQD) == 0)
		{
			ret = NBU_SUCCESS;
		}
	} while(0);

	return ret;
}

