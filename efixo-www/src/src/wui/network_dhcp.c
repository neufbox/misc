#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"
#include "nbd/lan.h"

#include <ezxml.h>

PAGE_DF("/network/dhcp", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        ezxml_t root, node;
        char *buf_xml = NULL;
        size_t buf_xml_size;
	int n;

        char parts[4][4];

        if (nbd_user_config_get("dhcp_active", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("dhcp_active", buffer);
        }

        if (nbd_user_config_get("dhcp_start", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                nbu_net_split_ip(buffer, parts);
                ewf_hdf_set("dhcp_start_p0", parts[0]);
                ewf_hdf_set("dhcp_start_p1", parts[1]);
                ewf_hdf_set("dhcp_start_p2", parts[2]);
                ewf_hdf_set("dhcp_start_p3", parts[3]);
        }

        if (nbd_user_config_get("dhcp_end", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                nbu_net_split_ip(buffer, parts);
                ewf_hdf_set("dhcp_end_p0", parts[0]);
                ewf_hdf_set("dhcp_end_p1", parts[1]);
                ewf_hdf_set("dhcp_end_p2", parts[2]);
                ewf_hdf_set("dhcp_end_p3", parts[3]);
        }

        if (nbd_user_config_get("dhcp_lease", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("dhcp_lease", buffer);
        }

        if (nbd_user_config_get("lan_ipaddr", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                nbu_net_split_ip(buffer, parts);
                ewf_hdf_set("dhcp_static_ipaddr_p0", parts[0]);
                ewf_hdf_set("dhcp_static_ipaddr_p1", parts[1]);
                ewf_hdf_set("dhcp_static_ipaddr_p2", parts[2]);
        }

#ifndef X86
        if(nbd_user_config_xml("dhcp_static", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(root != NULL)
		{
                        n = 0;
			for(node = ezxml_child(root, "pair"); node; node = node->next)
			{
				nbu_string_printf(buffer, sizeof buffer, "dhcp_static_list.%d.ipaddr", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "ipaddr"));

				nbu_string_printf(buffer, sizeof buffer, "dhcp_static_list.%d.macaddr", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "macaddr"));

				++n;
			}

			ezxml_free(root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }
                free(buf_xml);
        }
#endif

        return;
}

FORM_HANDLER
{
        int row, ret;
        char parts_lan_ip[4][4];

        char static_ipaddr[16], static_macaddr[18];

        char lan_ipaddr[16], lan_netmask[16];
        char dhcp_start[16], dhcp_end[16];
        int i_dhcp_start_p3 = 0, i_dhcp_end_p3 = 0;
        const char *action = NULL, *dhcp_active = NULL,
                *dhcp_start_p3 = NULL, *dhcp_end_p3 = NULL,
                *dhcp_lease = NULL, *dhcp_static_ipaddr_p3 = NULL,
                *dhcp_static_macaddr_p0 = NULL, *dhcp_static_macaddr_p1 = NULL,
                *dhcp_static_macaddr_p2 = NULL, *dhcp_static_macaddr_p3 = NULL,
                *dhcp_static_macaddr_p4 = NULL, *dhcp_static_macaddr_p5 = NULL;
        struct trx trx;

        NV_GET(lan_ipaddr);
        NV_GET(lan_netmask);

        EWF_HDF_GET_DEFAULT(action, "config");

        if (nbu_string_matches(action, "add") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(dhcp_static_ipaddr_p3);
                EWF_HDF_GET(dhcp_static_macaddr_p0);
                EWF_HDF_GET(dhcp_static_macaddr_p1);
                EWF_HDF_GET(dhcp_static_macaddr_p2);
                EWF_HDF_GET(dhcp_static_macaddr_p3);
                EWF_HDF_GET(dhcp_static_macaddr_p4);
                EWF_HDF_GET(dhcp_static_macaddr_p5);

                /* user added a rule */
                /* dhcp_static_ipaddr */
                if (nbu_net_split_ip(lan_ipaddr, parts_lan_ip) != NBU_SUCCESS)
                {
                        nbu_log_error("nbu_net_split_ip(%s, ..) failed",
                                      lan_ipaddr);
                        ewf_hdf_query_error("system");
                        return; /* RETURN */
                }

                if (nbu_net_join_ip(parts_lan_ip[0], parts_lan_ip[1],
                                    parts_lan_ip[2], dhcp_static_ipaddr_p3,
                                    static_ipaddr,
                                    sizeof(static_ipaddr)) != NBU_SUCCESS
                    || nbu_net_validate_full_ip(static_ipaddr,
                                                lan_netmask) != NBU_SUCCESS)
                {
                        ewf_hdf_query_var_error("dhcp_static_ipaddr",
                                                "invalid");
                        return; /* RETURN */
                }

                if(nbu_string_compare(static_ipaddr,
                                      lan_ipaddr) == NBU_STRING_EQUAL)
                {
                        ewf_hdf_query_var_error("dhcp_static_ipaddr",
                                                "boxip");
                        return;
                }

                /* dhcp_static_macaddr */
                if (nbu_net_join_mac(dhcp_static_macaddr_p0,
                                     dhcp_static_macaddr_p1,
                                     dhcp_static_macaddr_p2,
                                     dhcp_static_macaddr_p3,
                                     dhcp_static_macaddr_p4,
                                     dhcp_static_macaddr_p5,
                                     static_macaddr,
                                     sizeof(static_macaddr)) != NBU_SUCCESS)
                {
                        ewf_hdf_query_var_error("dhcp_static_macaddr",
                                                "invalid");
                        return;
                }

                /* add rule */
#ifndef X86
                ret = nbd_dhcpd_add_static_ip(static_ipaddr, static_macaddr);
                if(ret != NBD_SUCCESS)
                {
                        switch(ret)
                        {
                        case lan_error_ip_invalid :
                                ewf_hdf_set_error("dhcp_static_ipaddr",
                                                  "invalid");
                                break;

                        case lan_error_mac_invalid :
                                ewf_hdf_set_error("dhcp_static_macaddr",
                                                  "invalid");
                                break;

                        case lan_error_dhcpd_static_ip_already_exist :
                                ewf_hdf_set_error("dhcp_static_ipaddr",
                                                  "already_use");
                                break;

                        case lan_error_dhcpd_static_mac_already_exist:
                                ewf_hdf_set_error("dhcp_static_macaddr",
                                                  "already_use");
                                break;

                        default :
                                ewf_hdf_form_set_error("unknown");
                                break;

                        }
#endif
                        return;
                }

                nbd_sys_async_run(DHCPD_START_STOP,
                                  "restart");
                nbu_log_debug("restarting dhcpd");
        }
        /* user removed a rule */
        else if (nbu_string_compare(action, "remove") == NBU_SUCCESS)
        {
                EWF_HDF_GET_INT(row);
                row = row + 1;
#ifndef X86
                if (nbd_dhcpd_del_static_ip(row) == NBD_SUCCESS)
                {
                        nbd_sys_async_run(DHCPD_START_STOP,
                                          "restart");
                        nbu_log_debug("restarting dhcpd");
                }
#endif
        }
        else
        {
                EWF_HDF_GET(dhcp_active);
                EWF_HDF_GET(dhcp_start_p3);
                EWF_HDF_GET(dhcp_end_p3);
                EWF_HDF_GET(dhcp_lease);

                /* reconstruct correct ip addr for lan */
                if(nbu_net_split_ip(lan_ipaddr, parts_lan_ip) != NBU_SUCCESS)
                {
                        nbu_log_error("nbu_net_split_ip(%s, ..) failed",
                                      lan_ipaddr);
                        ewf_hdf_query_error("system");
                        return; /* RETURN */
                }

                /* check dhcp_start */
                if (nbu_net_join_ip(parts_lan_ip[0], parts_lan_ip[1],
                                    parts_lan_ip[2], dhcp_start_p3,
                                    dhcp_start,
                                    sizeof(dhcp_start)) != NBU_SUCCESS
                    || nbu_net_validate_full_ip(dhcp_start,
                                                lan_netmask) != NBU_SUCCESS)
                {
                        ewf_hdf_query_var_error("dhcp_start_p3", "invalid");
                        return; /* RETURN */
                }

                /* check dhcp_end */
                if (nbu_net_join_ip(parts_lan_ip[0], parts_lan_ip[1],
                                    parts_lan_ip[2], dhcp_end_p3,
                                    dhcp_end,
                                    sizeof(dhcp_end)) != NBU_SUCCESS
                    || nbu_net_validate_full_ip(dhcp_end,
                                                lan_netmask) != NBU_SUCCESS)
                {
                        ewf_hdf_query_var_error("dhcp_end_p3", "invalid");
                        return; /* RETURN */
                }

                /* check ip start and stop and gateway  */
                if (nbd_user_config_get("lan_ipaddr", buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        if (nbu_string_compare(buffer,dhcp_start)  == NBD_SUCCESS) {
                                ewf_hdf_query_var_error("dhcp_start_p3", "invalid");
                                return;
                        }
                        if (nbu_string_compare(buffer,dhcp_end) == NBD_SUCCESS) {
                                ewf_hdf_query_var_error("dhcp_end_p3", "invalid");
                                return;
                        }
                }

                /* another check */
                i_dhcp_start_p3 = strtol(dhcp_start_p3, NULL, 10);
                i_dhcp_end_p3 = strtol(dhcp_end_p3, NULL, 10);
                if(i_dhcp_start_p3 >= i_dhcp_end_p3)
                {
                        ewf_hdf_query_error("invalid_ip_range");
                        return; /* RETURN */
                }

                /* nv trx */
                NV_TRX_INIT(trx);

                NV_SET_WTRX(trx, "dhcp_active", dhcp_active);
                NV_SET_WTRX(trx, "dhcp_start", dhcp_start);
                NV_SET_WTRX(trx, "dhcp_end", dhcp_end);
                NV_SET_WTRX(trx, "dhcp_lease", dhcp_lease);

                if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
                {
                        nbd_sys_async_run(DHCPD_START_STOP,
                                          "restart");
                        nbu_log_debug("restarting dhcpd");
                }
        }

        return;
}
