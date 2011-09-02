#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbl_wlan.h"
#include "nbl_tv.h"

#include "nbd.h"

#include <ezxml.h>

PAGE_DFA_u("/network", EWF_ACCESS_PRIVATE);

DISPLAY_AJAX_HANDLER
{
        ezxml_t xmln_root, xmln_port, xmln_host;

        char *buf_xml = NULL;
        size_t buf_xml_size;

        int n;

        /* get info on lan interface */
        if(nbd_status_xml("link", &buf_xml, &buf_xml_size)  == NBD_SUCCESS)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        n = 0;
                        for(xmln_port = ezxml_child(xmln_root, "lan1"); xmln_port; xmln_port = xmln_port->ordered)
                        {

#if defined(CIBOX) || defined(NB4)
                                 /* get info on usb */
                                       /* no more usb in nb4 v2 and nb4 femto */
                                if ((nbu_string_compare(ezxml_name(xmln_port), "usb") == NBU_SUCCESS)
                                        && (nbd_status_get("productID", buffer, sizeof(buffer)) == NBD_SUCCESS))
                                {
                                        if(nbu_string_matches(buffer, "NB4-SER-r2") != NBU_STRING_EQUAL
                                           && nbu_string_matches(buffer, "NB4-FXC-r2") != NBU_STRING_EQUAL
                                           && nbu_string_matches(buffer, "NB4F_SER-r") != NBU_STRING_EQUAL)
                                        {
                                                if (nbd_status_get("link_usb_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
                                                {
                                                        ewf_hdf_set("USBStatus", buffer);
                                                }
                                        }
                                        continue;
                                }
#endif

                                nbu_string_printf(buffer, sizeof buffer, "links.%d.name", n);
                                ewf_hdf_set(buffer, ezxml_name(xmln_port));

                                nbu_string_printf(buffer, sizeof buffer, "links.%d.type", n);
                                ewf_hdf_set(buffer, "host");

                                nbu_string_printf(buffer, sizeof buffer, "links.%d.status", n);
                                ewf_hdf_set(buffer, ezxml_child_txt(xmln_port, "status"));

                                nbu_string_printf(buffer, sizeof buffer, "links.%d.speed", n);
                                ewf_hdf_set(buffer, ezxml_child_txt(xmln_port, "speed"));

                                ++n;

                        }

                        ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }

                free(buf_xml);
        }

        /* get info on tv */
#if defined(CIBOX)
        if (nbd_user_config_get("tv_ifname", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("TVIfname", buffer);
        }
#endif

#if defined(CIBOX)
        if (nbd_user_config_get("tv_config", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("TVConfig", buffer);
        }
#endif

#if defined(CIBOX)
        if (nbl_tv_get_status(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("TVStatus", buffer);
        }
#endif

        /* get info on wifi */
        if (nbd_wlan_active(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("WifiStatus", buffer);
        }

        /* lan topology */
        if(nbd_status_xml("lan_topology", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
		{
                        n = 0;
			for(xmln_host = ezxml_child(xmln_root, "device"); xmln_host; xmln_host = xmln_host->next)
			{
				if( (nbu_string_compare(ezxml_child_txt(xmln_host, "type"), "pc") == NBU_SUCCESS) || (nbu_string_compare(ezxml_child_txt(xmln_host, "type"), "stb") == NBU_SUCCESS ))
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

#ifndef X86
        ewf_hdf_setf("WifiCountClient", "%d", nbd_wlan_assoc_count(NULL));
#endif

        return;
}

FORM_HANDLER
{
        struct trx trx;
        const char *stbconfig = NULL,
                *stbconfig_iface = NULL;

        EWF_HDF_GET(stbconfig);
        EWF_HDF_GET(stbconfig_iface);

        NV_TRX_INIT(trx);

        NV_SET_WTRX(trx, "tv_config", stbconfig);
        NV_SET_WTRX(trx, "tv_ifname", stbconfig_iface);

        if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
        {
                nbd_sys_async_run(TV_START_STOP, "restart");
        }
}
