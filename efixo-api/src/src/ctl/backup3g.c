#include "api_common.h"

API_REGISTER_METHOD( backup3g, forceDataLink, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;

        if ( ewf_fastcgi_get_parameter( "mode", &param ) == EWF_SUCCESS )
	{
		nbd_nv_set( "gprs_service_data", param );
		nbd_nv_commit( "user" );
	}
}

API_REGISTER_METHOD( backup3g, forceVoipLink, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
        char *param = NULL;

        if ( ewf_fastcgi_get_parameter( "mode", &param ) == EWF_SUCCESS )
	{
		nbd_nv_set( "gprs_service_voice", param );
		nbd_nv_commit( "user" );
	}
}

API_REGISTER_METHOD( backup3g, getPinCode, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE )
{
        if(nbd_nv_get("gprs_pin", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                api_set_value("backup3g.getPinCode", buffer);
        }
}

API_REGISTER_METHOD( backup3g, setPinCode, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
        char *param = NULL;

	if (ewf_fastcgi_get_parameter("pincode", &param) == EWF_SUCCESS)
	{
		if (nbd_nv_set("gprs_pin", param) == NBD_SUCCESS)
		{
			nbd_nv_commit("user");
		}
	}
}
