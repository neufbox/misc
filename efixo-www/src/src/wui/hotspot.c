#include <string.h>
#include <ezxml.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbd.h"
#include "dbl.h"

#include <ezxml.h>

PAGE_DA("/hotspot", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        ezxml_t root, node;
        char *buf_xml = NULL;
        size_t buf_xml_size;
	int n;

        if(nbd_autoconf_matches("hotspot_enable", "false"))
        {
                nbu_log_notice("hotspot is disabled.");
                ewf_hdf_set("hotspot_enable", "false");
        }

        status_to_hdf("hotspot_status", "hotspot_status");
        nv_to_hdf("hotspot_active", "hotspot_active");

        if(nbd_user_config_matches("hotspot_active", "off"))
        {
                /* Use status off when hotspot is deactivated */
                ewf_hdf_owset("hotspot_status", "off");
        }

        nv_to_hdf("hotspot_mode", "hotspot_mode");

        if(nbd_hotspot_ssid(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("hotspot_ssid", buffer);
        }

        if(nbd_status_xml("hotspot", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(root != NULL)
		{
                        n = 0;
			for(node = ezxml_child(root, "assoc"); node; node = node->next)
			{
				++n;

				nbu_string_printf(buffer, sizeof buffer, "hotspot_assoc_list.%d.ipaddr", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "ipaddr"));

				nbu_string_printf(buffer, sizeof buffer, "hotspot_assoc_list.%d.macaddr", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "macaddr"));
			}

			ezxml_free(root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }
                free(buf_xml);
        }

	return;
}

AJAX_HANDLER
{
        if(nbd_autoconf_get("hotspot_enable", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("hotspot_enable", buffer);
	}

	if(nbd_user_config_get("hotspot_active", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("hotspot_active", buffer);
	}

	if(nbd_status_get("hotspot_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
                ewf_hdf_set("hotspot_status", buffer);
	}

        if(nbd_hotspot_ssid(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("hotspot_ssid", buffer);
        }

	return;
}
