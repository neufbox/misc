#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbl_net.h"
#include "nbl_wlan.h"

#include "nbd.h"

#include <ezxml.h>

PAGE_DF("/wifi/macfiltering", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	int n;

        char *buf_xml = NULL;
	size_t buf_xml_size;
	ezxml_t xmln_root, xmln_mac;

#ifndef X86
	if (nbd_wlan_macfiltering_mode(buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("MACFiltering", buffer);
	}
#endif

#ifndef X86
        if( nbd_user_config_xml("wlan_filter", &buf_xml, &buf_xml_size) == 0 )
	{
		xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
		if(xmln_root != NULL)
		{
			n = 0;

			for(xmln_mac = ezxml_child(xmln_root, "mac");
                            xmln_mac;
                            xmln_mac = xmln_mac->next)
			{
                                ++n;

                                nbu_string_printf(buffer, sizeof buffer, "List.%d", n);
                                ewf_hdf_set(buffer, ezxml_txt(xmln_mac));
                        }

			ezxml_free(xmln_root);
                }

                free(buf_xml);
        }
#endif

	return;
}

FORM_HANDLER
{
	const char *mac_filtering = NULL,
                *action = NULL;
	int restart = 0, ret;

	mac_filtering = ewf_hdf_sget("mac_filtering");
	if (mac_filtering != NULL)
	{
#ifndef X86
		nbd_wlan_macfiltering(mac_filtering);
#endif
		restart ++;
	}
	else
	{
		action = ewf_hdf_sget("action");
		if (nbu_string_matches(action, "add") == NBU_STRING_EQUAL)
		{
			ret = NBD_ERROR;
			if (nbu_net_join_mac(ewf_hdf_get("mac_address_p0"),
					     ewf_hdf_get("mac_address_p1"),
					     ewf_hdf_get("mac_address_p2"),
					     ewf_hdf_get("mac_address_p3"),
					     ewf_hdf_get("mac_address_p4"),
					     ewf_hdf_get("mac_address_p5"),
					     buffer, sizeof(buffer)) == NBU_SUCCESS
			    && nbu_net_validate_mac(buffer) == NBU_SUCCESS
			    && (ret = nbd_wlan_add_mac(buffer)) == NBD_SUCCESS)
			{
				restart++;
			}
			else if(ret == wlan_error_mac_already_used)
			{
				ewf_hdf_set_error("mac_address", "alreadyused");
			}
			else
			{
				ewf_hdf_set_error("mac_address", "invalid");
			}
		}
		else if (nbu_string_matches(action, "remove") == NBU_STRING_EQUAL)
		{
			if(ewf_hdf_sget_copy("mac_address", buffer, sizeof(buffer)) == EWF_SUCCESS)
			{
#ifndef X86
				nbd_wlan_del_mac(buffer);
#endif
				restart ++;
			}
		}
	}

	if (restart > 0)
	{
		nbd_user_config_commit();
	}

	return;
}
