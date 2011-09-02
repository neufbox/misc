#include "api_common.h"

#include "nbl_trash.h"

/* GET
 * productID
 * mac_addr
 * net_mode
 * uptime
 */
API_REGISTER_METHOD( system, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC )
{
	if ( nbd_status_get( "productID", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "system.product_id", buffer );
	}
	
	if ( nbd_status_get( "mac_addr", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "system.mac_addr", buffer );
	}

	api_set_value( "system.net_mode", "router" );
	
        if (nbd_status_get("net_data_access", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                api_set_value("system.net_infra", buffer);
        }
        
	if ( nbd_status_get( "uptime", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "system.uptime", buffer );
	}
	
	if ( nbd_status_get("version_mainfirmware", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("system.firmware_version", buffer);
	}
	
	if ( nbd_status_get("version_rescuefirmware", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("system.rescue_version", buffer);
	}
	
	if ( nbd_status_get("version_bootloader", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("system.bootloader_version", buffer);
	}

#ifndef NB5
        if (nbd_status_get("version_dsldriver", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                api_set_value("system.dsldriver_version", buffer);
        }
#endif
}

API_REGISTER_METHOD( system, getWpaKey, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE )
{
	if ( nbd_status_get("wpa_key", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("system.wpa_key", buffer);
	}
}

API_REGISTER_METHOD( system, setNetMode, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;

	if ( ewf_fastcgi_get_parameter( "mode", &param ) == EWF_SUCCESS ) 
	{
	}
}

API_REGISTER_METHOD( system, reboot, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	pid_t pid;

	pid = fork();
	if(pid == 0)
	{
		sleep(2);
		nbd_connect();
		nbd_sys_reboot();
		nbd_disconnect();
		exit(EXIT_SUCCESS);
	}
	else if(pid == -1)
	{
		//nbu_log_error("An error occur when trying to fork ! %s", strerror(errno));
	}
}
