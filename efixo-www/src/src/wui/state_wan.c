#include <string.h>
#include <ezxml.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include <nbd/config.h>
#include <nbd/dsl.h>
#include <nbd/backup3g.h>

#include <nbu/nbu.h>
#include <nbu/string.h>

#include "nbl.h"
#include "nbl_utils.h"
#include "nbl_net.h"
#include "nbd.h"
#include "dbl.h"

PAGE_D("/state/wan", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        char dns1[NBU_NET_SIZEOF_IPADDR], dns2[NBU_NET_SIZEOF_IPADDR];
        char infra[16], dhcp_voip[8], wan_link[8];
        int opt1_flag = 0;

        int timestamp;
        nbl_utils_ltime_t ltm;

        ezxml_t xmln_root, xmln_node;
	char *buf_xml = NULL;
	size_t buf_xml_size;

        status_to_hdf("net_data_status", "wan_status");
        status_to_hdf("net_data_ipaddr", "wan_ipaddr");
        status_to_hdf("net_data_gateway", "wan_gateway");
        status_to_hdf("net_data_uptime", "wan_uptime");
        status_to_hdf("adsl_status", "infra_dsl_status");
        
        if(nbd_status_get("net_data_uptime",
                          buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                errno = 0;
                timestamp = strtol(buffer, NULL, 10);
                if(errno == 0)
                {
                        nbl_utils_timestamp2ltime(timestamp, &ltm);

                        ewf_hdf_set_int("wan_uptime.sec",
                                        ltm.sec);
                        ewf_hdf_set_int("wan_uptime.min",
                                        ltm.min);
                        ewf_hdf_set_int("wan_uptime.hour",
                                        ltm.hour);
                        ewf_hdf_set_int("wan_uptime.day",
                                        ltm.day);
                }
        }

        if(nbd_status_get("net_data_dns",
                          buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                nbl_utils_strcut(buffer,
                                 ' ',
                                 dns1, sizeof(dns1),
                                 dns2, sizeof(dns2));
                ewf_hdf_set("wan_dns_1", dns1);
                ewf_hdf_set("wan_dns_2", dns2);
	}

        if(nbd_status_get("net_data_access", infra, sizeof(infra)) != NBU_SUCCESS)
        {
                infra[0] = '\0';
        }

        ewf_hdf_set("infra_name", infra);
        
        if(has_net_infra_adsl()
           && nbd_status_matches("adsl_status", "up"))
        {
                /* need net_mode and dhcpc_voip_status to compute
                 * protocol used
                 */

                if(nbd_status_get("dhcpc_voip_status",
                                  dhcp_voip,
                                  sizeof(dhcp_voip)) != NBD_SUCCESS)
                {
                        dhcp_voip[0] = '\0';
                }

                if(nbu_string_matches(dhcp_voip, "up") == NBU_STRING_EQUAL)
                {
                        opt1_flag = 1;
                }

		ewf_hdf_set("infra_proto",
				opt1_flag ? "pppoe_dhcp" : "pppoe");

                status_to_hdf("adsl_status",
                                 "infra_dsl_status");

                if(nbd_status_get("adsl_uptime",
                                  buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        errno = 0;
                        timestamp = strtol(buffer, NULL, 10);
                        if(errno == 0)
                        {
                                nbl_utils_timestamp2ltime(timestamp, &ltm);

                                ewf_hdf_set_int("infra_dsl_uptime.sec",
                                                ltm.sec);
                                ewf_hdf_set_int("infra_dsl_uptime.min",
                                                ltm.min);
                                ewf_hdf_set_int("infra_dsl_uptime.hour",
                                                ltm.hour);
                                ewf_hdf_set_int("infra_dsl_uptime.day",
                                                ltm.day);
                        }
                }

                if(nbd_dsl_get("adsl_rate_up",
                               buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        ewf_hdf_set("infra_dsl_rateup", buffer);
                }

                if(nbd_dsl_get("adsl_rate_down",
                               buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        ewf_hdf_set("infra_dsl_ratedown", buffer);
                }

                if(nbd_dsl_get("adsl_noise_up",
                               buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        ewf_hdf_set("infra_dsl_noiseup", buffer);
                }

                if(nbd_dsl_get("adsl_noise_down",
                               buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        ewf_hdf_set("infra_dsl_noisedown", buffer);
                }

                if(nbd_dsl_get("adsl_attenuation_up",
                               buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        ewf_hdf_set("infra_dsl_attenuationup", buffer);
                }

                if(nbd_dsl_get("adsl_attenuation_down",
                               buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        ewf_hdf_set("infra_dsl_attenuationdown", buffer);
                }

                if(nbd_dsl_get("adsl_linemode",
                               buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        ewf_hdf_set("infra_dsl_linemode", buffer);
                }

                /* ppp */
                status_to_hdf("ppp_adsl_status",
                                 "infra_ppp_status");
                status_to_hdf("ppp_adsl_ifname",
                                 "infra_ppp_ifname");
                status_to_hdf("ppp_adsl_session",
                                 "infra_ppp_session");
                status_to_hdf("ppp_adsl_server",
                                 "infra_ppp_server");
        }
        if(has_net_infra_ftth()
                && nbu_string_matches(infra, "ftth") == NBU_STRING_EQUAL)
        {
                ewf_hdf_set("infra_proto", "dhcp");

                if(nbd_status_get("ftth_access",
                                  wan_link,
                                  sizeof(wan_link)) != NBD_SUCCESS)
                {
                        wan_link[0] = '\0';
                }

#if defined(NB5)

                if(nbu_string_matches(wan_link,
                                      "access0") == NBU_STRING_EQUAL)
                {
                        ewf_hdf_set("infra_ftth_access",
                                    "sfp");

                        status_to_hdf("link_wan0_status",
                                         "infra_ftth_sfp_status");

                        status_to_hdf("link_wan0_speed",
                                         "infra_ftth_sfp_speed");

                        if(nbd_sfp_get("serial_base_wavelength",
                                       buffer,
                                       sizeof(buffer)) == NBD_SUCCESS)
                        {
                                ewf_hdf_set("infra_ftth_sfp_wavelength",
                                            buffer);
                        }

                        if(nbd_sfp_get("monitoring_temperature_value",
                                       buffer,
                                       sizeof(buffer)) == NBD_SUCCESS)
                        {
                                ewf_hdf_set("infra_ftth_sfp_temperature",
                                            buffer);
                        }
                }
                else if(nbu_string_matches(wan_link,
                                           "access1") == NBU_STRING_EQUAL)
                {
#endif
                        ewf_hdf_set("infra_ftth_access",
                                    "extern");

                        status_to_hdf("link_lan0_status",
                                         "infra_ftth_extern_status");

                        status_to_hdf("link_lan0_speed",
                                         "infra_ftth_extern_speed");
#if defined(NB5)
                }
                else
                {
                        ewf_hdf_set("infra_ftth_access",
                                    "unknown");
                }
#endif
        }
        if(has_net_infra_gprs()
                && nbu_string_matches(infra, "gprs") == NBU_STRING_EQUAL)
        {
                ewf_hdf_set("infra_proto", "pppoe");

                if(nbd_backup3g_getInfo(&buf_xml,
                                        &buf_xml_size) == NBD_SUCCESS)
                {
                        xmln_root = ezxml_parse_str(buf_xml,
                                                    buf_xml_size);
                        if(xmln_root != NULL)
                        {
                                xmln_node = ezxml_get(xmln_root,
                                                      "rssi", -1);
                                ewf_hdf_set("infra_gprs_rssi",
                                            ezxml_txt(xmln_node));

                                xmln_node = ezxml_get(xmln_root,
                                                      "dbm", -1);
                                ewf_hdf_set("infra_gprs_dbm",
                                            ezxml_txt(xmln_node));

                                xmln_node = ezxml_get(xmln_root,
                                                      "ber", -1);
                                ewf_hdf_set("radio_gprs_ber",
                                            ezxml_txt(xmln_node));

                                ezxml_free(xmln_root);
                        }
                        else
                        {
                                nbu_log_error("Unable to parse xml string");
                        }

                        free(buf_xml);
                }

                /* ppp */
                status_to_hdf("ppp_gprs_status",
                                 "infra_ppp_status");
                status_to_hdf("ppp_gprs_ifname",
                                 "infra_ppp_ifname");
                status_to_hdf("ppp_gprs_session",
                                 "infra_ppp_session");
                status_to_hdf("ppp_gprs_server",
                                 "infra_ppp_server");
        }

        //IPv6
        STATUS_TO_HDF("ipv6_status");
        STATUS_TO_HDF("net_ipv6_prefix");
        STATUS_TO_HDF("net_ipv6_ipaddr");
        STATUS_TO_HDF("ipv6_lock");
        NV_TO_HDF("ipv6_enable");
        if(nbd_status_get("net_ipv6_uptime", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                errno = 0;
                timestamp = strtol(buffer, NULL, 10);
                if(errno == 0)
                {
                        nbl_utils_timestamp2ltime(timestamp, &ltm);

                        ewf_hdf_set_int("net_ipv6_uptime.sec",
                                        ltm.sec);
                        ewf_hdf_set_int("net_ipv6_uptime.min",
                                        ltm.min);
                        ewf_hdf_set_int("net_ipv6_uptime.hour",
                                        ltm.hour);
                        ewf_hdf_set_int("net_ipv6_uptime.day",
                                        ltm.day);
                }
        }

}
