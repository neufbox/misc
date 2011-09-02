#include "api_common.h"

API_REGISTER_METHOD( firewall, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE )
{
	if ( nbd_nv_get( "filter_mode", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "firewall.filter_mode", buffer );
	}
	
	if ( nbd_nv_get( "filter_simple_winsharedrop", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "firewall.filter_simple_winsharedrop", buffer );
	}
	
	if ( nbd_nv_get( "filter_simple_icmpdrop", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "firewall.filter_simple_icmpdrop", buffer );
	}
	
	if ( nbd_nv_get( "filter_simple_smtpdrop", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "firewall.filter_simple_smtpdrop", buffer );
	}
}

API_REGISTER_METHOD( firewall, enableSmtpFilter, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	nbd_nv_set( "filter_simple_smtpdrop", "on" );
	nbd_nv_commit( "user" );

	nbd_sys_sync_run(FIREWALL_START_STOP, "smtp");
}

API_REGISTER_METHOD( firewall, disableSmtpFilter, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	nbd_nv_set( "filter_simple_smtpdrop", "off" );
	nbd_nv_commit( "user" );
	
	nbd_sys_sync_run(FIREWALL_START_STOP, "smtp");
}
