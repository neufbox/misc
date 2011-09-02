#include "api_common.h"

#include "nbl_trash.h"

API_REGISTER_METHOD(wan, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC)
{
        
	if (nbd_status_get("net_data_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("wan.status", buffer);
	}

	if (nbd_status_get("net_data_uptime", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("wan.uptime", buffer);
	}
	
	if (nbd_status_get("net_data_ipaddr", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("wan.ip_addr", buffer);
	}

        if (nbd_status_get("net_data_access", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                api_set_value("wan.infra", buffer);
        }
}
