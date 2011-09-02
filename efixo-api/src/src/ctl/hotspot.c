#include "api_common.h"

#include <ezxml.h>

/* GET
 * hotspot_status
 * hotspot_active
 * hotspot_mode
 */
API_REGISTER_METHOD(hotspot, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE)
{
	if (nbd_status_get("hotspot_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("hotspot.status", buffer);
	}

	if (nbd_nv_get("hotspot_active", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("hotspot.enabled", buffer);
	}

	if (nbd_nv_get("hotspot_mode", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("hotspot.mode", buffer);
	}
}

API_REGISTER_METHOD(hotspot, getClientList, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE)
{
        ezxml_t root, node;
        char *buf_xml = NULL;
        size_t buf_xml_size;
	int n;

        if(nbd_status_xml("hotspot", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(root != NULL)
		{
                        n = 0;
			for(node = ezxml_child(root, "assoc"); node; node = node->next)
			{
				++n;

				nbu_string_printf(buffer, sizeof buffer, "hotspot_assoc_list.%d.ipaddr", n);
				api_set_value(buffer, ezxml_child_txt(node, "ipaddr"));

				nbu_string_printf(buffer, sizeof buffer, "hotspot_assoc_list.%d.macaddr", n);
				api_set_value(buffer, ezxml_child_txt(node, "macaddr"));
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

/* SET
 * hotspot_active
 * hotspot_mode
 */
API_REGISTER_METHOD(hotspot, setMode, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
	char *param = NULL;

	if (ewf_fastcgi_get_parameter("mode", &param) == EWF_SUCCESS)
	{
		nbd_nv_set("hotspot_mode", param);
		nbd_nv_commit("user");
	}
}

API_REGISTER_METHOD(hotspot, enable, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
	nbd_nv_set("hotspot_active", "on");
	nbd_nv_commit("user");
}

API_REGISTER_METHOD(hotspot, disable, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
	nbd_nv_set("hotspot_active", "off");
	nbd_nv_commit("user");
}

/* ACTION
 * start
 * stop
 * restart
 */
API_REGISTER_METHOD(hotspot, start, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
	nbd_hotspot_start( );
}

API_REGISTER_METHOD(hotspot, stop, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
	nbd_hotspot_stop( );
}

API_REGISTER_METHOD(hotspot, restart, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
	nbd_hotspot_restart( );
}
