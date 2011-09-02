#include "api_common.h"
#include <ezxml.h>

API_REGISTER_METHOD( storage, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE )
{
        char *buf_xml = NULL;
        size_t buf_xml_size;
        ezxml_t xmln_root;
        
        /* get status of service */
	if( nbd_sambactl_status(&buf_xml, &buf_xml_size) == NBD_SUCCESS )
	{
		xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
		if(xmln_root != NULL)
		{
			api_set_value("storage.sambaactive", ezxml_child_txt(xmln_root, "active"));
			api_set_value("storage.sambastate", ezxml_child_txt(xmln_root, "state"));
 			api_set_value("storage.sambacodeno", ezxml_child_txt(xmln_root, "codeno"));
			ezxml_free(xmln_root);
		}
		else
		{
			nbu_log_error("Unable to parse xml string");
		}

		free(buf_xml);
	}
}