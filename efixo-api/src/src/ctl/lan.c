#include "api_common.h"
#include <ezxml.h>

API_REGISTER_METHOD(lan, getHostsList, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC)
{
        char * buf_xml = NULL;
        size_t buf_xml_size;
        ezxml_t xmln_root, xmln_host;
        int n = 0;
        
        /* lan topology */
        if(nbd_status_xml("lan_topology", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
		{
                        n = 0;
			for(xmln_host = ezxml_child(xmln_root, "device"); xmln_host; xmln_host = xmln_host->next)
			{

				if(
                                        (nbu_string_compare(ezxml_child_txt(xmln_host, "type"), "pc") == NBU_SUCCESS )
                                         || (nbu_string_compare(ezxml_child_txt(xmln_host, "type"), "stb") == NBU_SUCCESS )
                                   )
				{
                                	++n;

					nbu_string_printf(buffer, sizeof buffer, "lan.hosts.%d.name", n);
					api_set_value(buffer, ezxml_child_txt(xmln_host, "hostname"));

					nbu_string_printf(buffer, sizeof buffer, "lan.hosts.%d.ip", n);
					api_set_value(buffer, ezxml_child_txt(xmln_host, "ipaddr"));

					nbu_string_printf(buffer, sizeof buffer, "lan.hosts.%d.mac", n);
					api_set_value(buffer, ezxml_child_txt(xmln_host, "macaddr"));

					nbu_string_printf(buffer, sizeof buffer, "lan.hosts.%d.iface", n);
					api_set_value(buffer, ezxml_child_txt(xmln_host, "port"));
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
}

API_REGISTER_METHOD(lan, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC)
{
	char parts[4][4];
        
	if (nbd_nv_get("lan_ipaddr", buffer, sizeof(buffer)) == NBD_SUCCESS) 
	{
		api_set_value("lan.ipaddr", buffer);
	}
	
	if(nbd_nv_get("lan_netmask", buffer, sizeof(buffer)) == NBD_SUCCESS) 
	{
		api_set_value("lan.netmask", buffer);
	}

        if (nbd_nv_get("dhcp_active", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("lan.dhcpactive", buffer);
	}

	if (nbd_nv_get("dhcp_start", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		nbu_net_split_ip(buffer, parts);
		api_set_value("lan.dhcpstartp0", parts[0]);
		api_set_value("lan.dhcpstartp1", parts[1]);
		api_set_value("lan.dhcpstartp2", parts[2]);
		api_set_value("lan.dhcpstartp3", parts[3]);
	}

	if (nbd_nv_get("dhcp_end", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		nbu_net_split_ip(buffer, parts);
		api_set_value("lan.dhcpendp0", parts[0]);
		api_set_value("lan.dhcpendp1", parts[1]);
		api_set_value("lan.dhcpendp2", parts[2]);
		api_set_value("lan.dhcpendp3", parts[3]);
	}

	if (nbd_nv_get("dhcp_lease", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("lan.dhcplease", buffer);
	}
}
