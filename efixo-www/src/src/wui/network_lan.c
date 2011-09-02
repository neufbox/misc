#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>

#include "webapp.h"

#include "nbl.h"
#include "nbd.h"

PAGE_DF("/network/lan", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	char ip_parts[4][4];
	char netmask_parts[4][4];

	if (nbd_user_config_get("lan_ipaddr", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		nbu_net_split_ip(buffer, ip_parts);
		ewf_hdf_set("lan_ipaddr_start_part1", ip_parts[0]);
		ewf_hdf_set("lan_ipaddr_start_part2", ip_parts[1]);
		ewf_hdf_set("lan_ipaddr_start_part3", ip_parts[2]);
		ewf_hdf_set("lan_ipaddr_start_part4", ip_parts[3]);
	}

	if(nbd_user_config_get("lan_netmask", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		nbu_net_split_ip(buffer, netmask_parts);
		ewf_hdf_set("lan_netmask_start_part1", netmask_parts[0]);
		ewf_hdf_set("lan_netmask_start_part2", netmask_parts[1]);
		ewf_hdf_set("lan_netmask_start_part3", netmask_parts[2]);
		ewf_hdf_set("lan_netmask_start_part4", netmask_parts[3]);
	}

	return;
}

FORM_HANDLER
{
	char buf_ip[16], buf_netmask[16];
	int ret;

	/* nbu_net_join_ip check if arguments is != to NULL */

	if(nbu_net_join_ip(ewf_hdf_sget("lan_ipaddr_start_part1"),
			   ewf_hdf_sget("lan_ipaddr_start_part2"),
			   ewf_hdf_sget("lan_ipaddr_start_part3"),
			   ewf_hdf_sget("lan_ipaddr_start_part4"),
			   buf_ip, sizeof buf_ip) != NBU_SUCCESS)
	{
		/* invalid ip */
		ewf_hdf_set_error("lan_ipaddr", "invalid_ip");

		return;
	}

	if(nbu_net_join_ip(ewf_hdf_sget("lan_netmask_start_part1"),
			   ewf_hdf_sget("lan_netmask_start_part2"),
			   ewf_hdf_sget("lan_netmask_start_part3"),
			   ewf_hdf_sget("lan_netmask_start_part4"),
			   buf_netmask, sizeof buf_netmask) != NBU_SUCCESS)
	{
		/* invalid mask */
		ewf_hdf_set_error("lan_netmask", "invalid_netmask");

		return;
	}

	/* variables is CLEAN here */

	if((ret = nbu_net_validate_full_ip(buf_ip, buf_netmask)) != NBU_SUCCESS)
	{
		/* invalid ip or netmask */
		if(ret == nbu_net_err_invalid_ip)
		{
			ewf_hdf_set_error("lan_ipaddr", "invalid_ip");
		}
		else if(ret == nbu_net_err_invalid_mask)
		{
			ewf_hdf_set_error("lan_netmask", "invalid_netmask");
		}
		else if(ret == nbu_net_err_ip_is_network)
		{
			ewf_hdf_set_error("lan_ipaddr", "ip_is_network");
		}
		else if(ret == nbu_net_err_ip_is_broadcast)
		{
			ewf_hdf_set_error("lan_ipaddr", "ip_is_broadcast");
		}
		else
		{
			ewf_hdf_set_error("lan_ipaddr", "error");
			ewf_hdf_set_error("lan_netmask", "error");
		}

		return;
	}

	/* check if ip isn't in hotspot network */
	if(strncmp(buf_ip, "192.168.", 8) == 0
	   && strtol(ewf_hdf_sget("lan_ipaddr_start_part3"), NULL, 10) == 2)
	{
		/* ip is in hotspot network ! */
		ewf_hdf_set_error("lan_ipaddr", "ip_is_in_hotspot_net");

		return;
	}

	if(nbd_lan_set_ipaddr(buf_ip, buf_netmask) == NBD_SUCCESS)
	{
		nbd_user_config_commit();
		ewf_utils_redirect(WEBAPP_URI_REBOOT);
	}
	else
	{
		ewf_hdf_form_set_error("error");
	}

	return;
}
