#include "api_common.h"

#include "nbd/dsl.h"

/* OK */

/* GET
 * adsl_status
 * adsl_noise_down
 * adsl_noise_up
 * adsl_attenuation_down
 * adsl_attenuation_up
 * adsl_rate_down
 * adsl_rate_up
 */
API_REGISTER_METHOD(dsl, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC)
{
        if (nbd_status_get("adsl_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.status", buffer);
	}
        
	if (nbd_status_get("adsl_uptime", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.uptime", buffer);
	}
        
	if (nbd_status_get("adsl_counter", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.counter", buffer);
	}
	
	if (nbd_dsl_get("adsl_noise_down", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.noise_down", buffer);
	}
	
	if (nbd_dsl_get("adsl_noise_up", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.noise_up", buffer);
	}
	
	if (nbd_dsl_get("adsl_attenuation_down", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.attenuation_down", buffer);
	}
	
	if (nbd_dsl_get("adsl_attenuation_up", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.attenuation_up", buffer);
	}
	
	if (nbd_dsl_get("adsl_rate_down", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.rate_down", buffer);
	}
	
	if (nbd_dsl_get("adsl_rate_up", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.rate_up", buffer);
	}
	
	if (nbd_dsl_get("adsl_linemode", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.linemode", buffer);
	}
        
	if (nbd_dsl_get("adsl_crc", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("dsl.crc", buffer);
	}
}
