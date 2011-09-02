#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include <ezxml.h>

#include "nbl.h"
#include "nbl_wlan.h"
#include "nbd.h"
#include "dbl.h"

PAGE_D("/wifi", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	int n;

        char *buf_xml;
	size_t buf_xml_size;
	ezxml_t xmln_root, xmln_host;

        if(nbd_wlan_active(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("wifi_status", buffer);
        }

	nv_to_hdf("wlan_wl0_ssid", "wifi_ssid");
        nv_to_hdf("wlan_closed", "wifi_closed");

#ifndef NB5
	if (nbd_user_config_get("wlan_autochannel", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("wifi_autochannel", buffer);
	}
#endif

#ifndef X86
	if (nbd_wlan_get_current_channel(buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("wifi_channel", buffer);
	}
#endif

#if defined(NB5) || defined(NB6)
	if (nbd_user_config_get("wlan_ht40", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("wifi_ht40", buffer);
	}
#endif

	if (nbd_user_config_get("wlan_mode", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("wifi_mode", buffer);
	}

	if (nbd_user_config_get("wlan_wl0_enc", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("wifi_encryption", buffer);

		if (nbu_string_matches(buffer, "WEP") == NBU_STRING_EQUAL)
		{
			nv_to_hdf("wlan_wl0_wepkeys_n0", "wifi_key");
			nv_to_hdf("wlan_wl0_keytype", "wifi_keytype");
		}
		else if (nbu_string_matches(buffer, "WPA-PSK") == NBU_STRING_EQUAL ||
			 nbu_string_matches(buffer, "WPA2-PSK") == NBU_STRING_EQUAL ||
			 nbu_string_matches(buffer, "WPA-WPA2-PSK") == NBU_STRING_EQUAL )
		{
			nv_to_hdf("wlan_wl0_wpakey", "wifi_key");
			nv_to_hdf("wlan_wl0_enctype", "wifi_keytype");
		}
	}

#ifndef X86
	if (nbd_wlan_macfiltering_mode(buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("wifi_macfiltering", buffer);
	}
        
        /* lan topology */
        if(nbd_status_xml("lan_topology", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
		{
                        n = 0;
			for(xmln_host = ezxml_child(xmln_root, "device"); xmln_host; xmln_host = xmln_host->next)
			{
				if(nbu_string_compare(ezxml_child_txt(xmln_host, "port"), "wlan0") == NBU_SUCCESS)
				{
                                	++n;

					nbu_string_printf(buffer, sizeof buffer, "hosts.%d.name", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "hostname"));

					nbu_string_printf(buffer, sizeof buffer, "hosts.%d.ip", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "ipaddr"));

					nbu_string_printf(buffer, sizeof buffer, "hosts.%d.mac", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "macaddr"));

					nbu_string_printf(buffer, sizeof buffer, "hosts.%d.iface", n);
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

        
#endif

	return;
}
