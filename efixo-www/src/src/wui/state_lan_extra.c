#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"
#include "nbd/lan.h"

#include <ezxml.h>

PAGE_D("/state/lan/extra", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	int n;

	ezxml_t xmln_root, xmln_port, xmln_stat;

	char *buf_xml;
	size_t buf_xml_size;

#ifndef X86
	/* Get FULL stats for the lan ports */
	if(nbd_lan_fullstats(&buf_xml, &buf_xml_size, "lan") == NBD_SUCCESS)
	{
		xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
		if(xmln_root != NULL)
		{
			n = 0;
			for(xmln_port = ezxml_child(xmln_root, "port"); xmln_port; xmln_port = xmln_port->next)
			{
				++n;

				nbu_string_printf(buffer, sizeof buffer, "ports.%d.name", n);
				ewf_hdf_set(buffer, ezxml_attr(xmln_port, "name"));

				for(xmln_stat = ezxml_child(xmln_port, "counter"); xmln_stat; xmln_stat = xmln_stat->next)
				{
					nbu_string_printf(buffer, sizeof buffer, "ports.%d.counters.%s", n, ezxml_attr(xmln_stat, "name"));
					ewf_hdf_set(buffer, ezxml_txt(xmln_stat));
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
