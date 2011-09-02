#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_renderer.h>

#include <nbd/config.h>

#include "nbd.h"
#include "dbl.h"

#include <ezxml.h>

PAGE_D("/stb/info", EWF_ACCESS_PUBLIC);

DISPLAY_HANDLER
{
        ewf_renderer_force_content_type_output(CONTENT_TYPE_XML);

        char net_infra[256];

	ezxml_t xmln_root, xmln_host;

	char *buf_xml = NULL;
	size_t buf_xml_size;

	int n;

	/* iad section */
        status_to_hdf("version_mainfirmware", "version_mainfirmware");
        status_to_hdf("version_rescuefirmware", "version_rescuefirmware");
        status_to_hdf("productID", "productID");
        status_to_hdf("mac_addr", "mac_addr");

        if(nbd_status_get("net_tv_access",
                          net_infra, sizeof(net_infra)) == NBD_SUCCESS)
        {
                ewf_hdf_set("net_infra", net_infra);
        }
        else
        {
                nbu_log_error("set netctl_tv_infra to default (adsl)");
                nbu_string_copy(net_infra, sizeof(net_infra), "adsl");
        }

        ewf_hdf_set("net_mode", "router"); /* net_mode is always router */
        status_to_hdf("uptime", "uptime");

        /* wan section (dsl) and ppp section */
        if(has_net_infra_ftth()
           && nbu_string_matches(net_infra, "ftth") == NBU_STRING_EQUAL)
        {
                /* very static for now ... */
                ewf_hdf_set("dsl_profile", "1");

                ewf_hdf_set("dsl_downstreamrate", "20000");
                ewf_hdf_set("dsl_upstreamrate", "20000");
                ewf_hdf_set("dsl_downstreamnoise", "5");
                ewf_hdf_set("dsl_upstreamnoise", "5");
                ewf_hdf_set("dsl_downstreamattn", "5");
                ewf_hdf_set("dsl_upstreamattn", "5");
                ewf_hdf_set("dsl_linemode", "G.DMT");
                ewf_hdf_set("dsl_crc", "0");

                status_to_hdf("net_data_status", "dsl_status");
                status_to_hdf("net_data_uptime", "dsl_uptime");
                status_to_hdf("net_data_counter", "dsl_counter");

                status_to_hdf("net_data_status", "ppp_status");
                status_to_hdf("net_data_uptime", "ppp_uptime");
                status_to_hdf("net_data_counter", "ppp_counter");
                status_to_hdf("net_data_gateway", "ppp_server");
                status_to_hdf("net_data_ipaddr", "ppp_ipaddr");
                status_to_hdf("net_data_gateway", "ppp_gateway");
        }
        else if(has_net_infra_adsl())
        {
                nv_to_hdf("adsl_profile", "dsl_profile");

                status_to_hdf("adsl_status", "dsl_status");
                status_to_hdf("adsl_uptime", "dsl_uptime");
                status_to_hdf("adsl_counter", "dsl_counter");

                if( nbd_dsl_rate_down_get(buffer, sizeof buffer) == NBD_SUCCESS)
                {
                        ewf_hdf_set("dsl_downstreamrate", buffer);
                }

                if( nbd_dsl_rate_up_get(buffer, sizeof buffer) == NBD_SUCCESS)
                {
                        ewf_hdf_set("dsl_upstreamrate", buffer);
                }

                if( nbd_dsl_noise_down_get(buffer, sizeof buffer) == NBD_SUCCESS)
                {
                        ewf_hdf_set("dsl_downstreamnoise", buffer);
                }

                if( nbd_dsl_noise_up_get(buffer, sizeof buffer) == NBD_SUCCESS)
                {
                        ewf_hdf_set("dsl_upstreamnoise", buffer);
                }

                if( nbd_dsl_attenuation_down_get(buffer, sizeof buffer) == NBD_SUCCESS)
                {
                        ewf_hdf_set("dsl_downstreamattn", buffer);
                }

                if( nbd_dsl_attenuation_up_get(buffer, sizeof buffer) == NBD_SUCCESS)
                {
                        ewf_hdf_set("dsl_upstreamattn", buffer);
                }

                if( nbd_dsl_linemode_get(buffer, sizeof buffer) == NBD_SUCCESS)
                {
                        ewf_hdf_set("dsl_linemode", buffer);
                }

                if (nbd_dsl_crc_get(buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        ewf_hdf_set("dsl_crc", buffer);
                }

                status_to_hdf("ppp_adsl_status", "ppp_status");
                status_to_hdf("ppp_adsl_uptime", "ppp_uptime");
                status_to_hdf("ppp_adsl_counter", "ppp_counter");
                status_to_hdf("ppp_adsl_gateway", "ppp_server");
                status_to_hdf("ppp_adsl_ipaddr", "ppp_ipaddr");
                status_to_hdf("ppp_adsl_gateway", "ppp_gateway");
        }

        /* voip */
        status_to_hdf("voip_linestatus", "voip_linestatus");
        status_to_hdf("voip_hookstate", "voip_hookstate");

        /* lan */
        nv_to_hdf("lan_ipaddr", "lan_ipaddr");
        nv_to_hdf("lan_netmask", "lan_netmask");

	/* lan topology */
        if(nbd_status_xml("lan_topology", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
		{
                        n = 0;
			for(xmln_host = ezxml_child(xmln_root, "device"); xmln_host; xmln_host = xmln_host->next)
			{
				if(nbu_string_compare(ezxml_child_txt(xmln_host, "type"), "pc") == NBU_SUCCESS )
				{
                                	++n;

					nbu_string_printf(buffer, sizeof buffer, "lan_hosts.%d.name", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "hostname"));

					nbu_string_printf(buffer, sizeof buffer, "lan_hosts.%d.ip", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "ipaddr"));

					nbu_string_printf(buffer, sizeof buffer, "lan_hosts.%d.mac", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "macaddr"));

					nbu_string_printf(buffer, sizeof buffer, "lan_hosts.%d.port", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "port"));
				}
			}

			ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }
                free(buf_xml);
        }

	return;
}
