#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>

#include "webapp.h"

#include "nbl.h"
#include "nbd.h"

#include <ezxml.h>

PAGE_D("/lanpopup", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	int n;
	const char *iface = NULL, *iface_wanted = NULL;
	ezxml_t xmln_root, xmln_host;

	char *buf_xml = NULL;
	size_t buf_xml_size;

	/* get iface wanted by user */
	iface_wanted = ewf_hdf_sget("portfilter");
	if(iface_wanted != NULL)
	{
		ewf_hdf_set("PortFilter", iface_wanted);
	}

        /* lan topology */
        if(nbd_status_xml("lan_topology", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
		{
                        n = 0;
			for(xmln_host = ezxml_child(xmln_root, "device"); xmln_host; xmln_host = xmln_host->next)
			{
				iface = ezxml_child_txt(xmln_host, "port");

				if(
                                   (iface_wanted == NULL || nbu_string_compare(iface, iface_wanted) == NBU_SUCCESS)
                                    && nbu_string_compare(ezxml_child_txt(xmln_host, "type"), "pc") == NBU_SUCCESS )
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


	/* Ip client */
	ewf_hdf_set("RemoteAddress", ewf_utils_client_ip());
}
