#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"
#include <ezxml.h>

PAGE_D("/network/nat/upnp", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        ezxml_t root, node;
        char *buf_xml = NULL;
        size_t buf_xml_size;
	int n;

        if(nbd_nat_upnp_list(&buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(root != NULL)
		{
                        n = 0;
			for(node = ezxml_child(root, "rule"); node; node = node->next)
			{
				/* Proto */
				nbu_string_printf(buffer, sizeof buffer, "UpnpList.%d.Proto", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "proto"));

				/* Ext port */
				nbu_string_printf(buffer, sizeof buffer, "UpnpList.%d.ExtPort", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "extport"));

				/* Dst IP */
				nbu_string_printf(buffer, sizeof buffer, "UpnpList.%d.DstIP", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "dstip"));

				/* Dst port */
				nbu_string_printf(buffer, sizeof buffer, "UpnpList.%d.DstPort", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "dstport"));

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
}
