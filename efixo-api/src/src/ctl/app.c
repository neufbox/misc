#include "api_common.h"

#include <nbd/sambactl.h>

#include <ezxml.h>

API_REGISTER_METHOD( app, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC )
{
        char *buf_xml = NULL;
        size_t buf_xml_size;
        ezxml_t xmln_root;

        if (nbd_status_get("p910nd_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
                api_set_value("p910nd.status", buffer);
	}

        if (nbd_nv_get("p910nd_bidir", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
                api_set_value("p910nd.bidir", buffer);
	}

        /* get status of service */
        if( nbd_sambactl_status(&buf_xml, &buf_xml_size) == NBD_SUCCESS )
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        api_set_value("smbd.active",
                                      ezxml_child_txt(xmln_root, "active"));
			api_set_value("smbd.state",
                                      ezxml_child_txt(xmln_root, "state"));
                        api_set_value("smbd.codeno",
                                      ezxml_child_txt(xmln_root, "codeno"));
                        ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }

                free(buf_xml);
        }
}
