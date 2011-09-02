#include <string.h>

#include <ezxml.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include <nbd/sambactl.h>
#include <nbd/usharectl.h>

#include "nbl.h"
#include "nbd.h"
#include "dbl.h"

PAGE_D("/service", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	char *buf_xml = NULL;
	size_t buf_xml_size;

	ezxml_t xmln_root;

	if( nbd_sambactl_status(&buf_xml, &buf_xml_size) == NBD_SUCCESS )
	{
		xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
		if(xmln_root != NULL)
		{
			ewf_hdf_set("samba_active", ezxml_txt(ezxml_child(xmln_root, "active")));
			ewf_hdf_set("samba_state", ezxml_txt(ezxml_child(xmln_root, "state")));
			ewf_hdf_set("samba_codeno", ezxml_txt(ezxml_child(xmln_root, "codeno")));

			ezxml_free(xmln_root);
		}
		else
		{
			nbu_log_error("Unable to parse xml string");
		}

		free(buf_xml);
	}

	status_to_hdf("p910nd_status", "print_server_status");

	if( nbd_usharectl_status(&buf_xml, &buf_xml_size) == NBD_SUCCESS )
	{
		xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
		if(xmln_root != NULL)
		{
			ewf_hdf_set("upnpav_active", ezxml_txt(ezxml_child(xmln_root, "active")));
			ewf_hdf_set("upnpav_state", ezxml_txt(ezxml_child(xmln_root, "state")));
			ewf_hdf_set("upnpav_codeno", ezxml_txt(ezxml_child(xmln_root, "codeno")));

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
