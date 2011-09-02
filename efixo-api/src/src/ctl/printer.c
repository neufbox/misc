#include "api_common.h"

API_REGISTER_METHOD( printer, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE )
{
        if (nbd_status_get("p910nd_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("printer.p910nd_status", buffer);
	}

	if (nbd_nv_get("p910nd_bidir", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("printer.p910nd_bidir", buffer);
	}
}