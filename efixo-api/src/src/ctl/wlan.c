#include "api_common.h"
#include "cfg.h"

#include <net/if.h>

#include <ezxml.h>

#include <nbd/wlan.h>

API_REGISTER_METHOD( wlan, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE )
{
	if (nbd_wlan_macfiltering_mode(buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("wlan.macfiltering", buffer);
	}

        if ( nbd_wlan_active( buffer, sizeof(buffer) ) == NBD_SUCCESS )
	{
		api_set_value( "wlan.active", buffer );
	}

	if ( nbd_nv_get( "wlan_channel", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "wlan.channel", buffer );
	}

	if ( nbd_nv_get( "wlan_mode", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "wlan.mode", buffer );
	}

	if ( nbd_nv_get( "wlan_wl0_ssid", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "wlan.wl0.ssid", buffer );
	}

	if ( nbd_nv_get( "wlan_wl0_enc", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "wlan.wl0.enc", buffer );
	}

	if ( nbd_nv_get( "wlan_wl0_keytype", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "wlan.wl0.keytype", buffer );
	}

	if ( nbd_nv_get( "wlan_wl0_wpakey", buffer, sizeof(buffer) ) == NBD_SUCCESS ) 
	{
		api_set_value( "wlan.wl0.wpakey", buffer );
	}

	if ( nbd_nv_get( "wlan_wl0_wepkeys_n0" , buffer, sizeof(buffer) ) == NBD_SUCCESS ) {
		api_set_value( "wlan.wl0.wepkey", buffer );
	}
}

API_REGISTER_METHOD( wlan, getClientList, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE )
{
        char ipaddr[NBU_NET_SIZEOF_IPADDR];
        int ret;

        int n = 0;

        char *buf_xml = NULL;
        size_t buf_xml_size;
	ezxml_t xmln_root, xmln_mac;

	ret = nbd_wlan_list_assoc(NULL, 
                                  &buf_xml, 
                                  &buf_xml_size);

        if(ret == NBD_SUCCESS)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        n = 0;
                        
                        for(xmln_mac = ezxml_get(xmln_root, 
                                                 "assoclist", 
                                                 0, "mac", -1); 
                            xmln_mac; 
                            xmln_mac = xmln_mac->next)
                        {
                                ++n;
                                
                                nbu_string_printf( buffer, sizeof buffer, "wlan.clients.%d.mac_addr", n );
                                api_set_value(buffer, ezxml_txt(xmln_mac));

                                if(nbd_lan_mac2ip(ezxml_txt(xmln_mac), 
                                                  ipaddr, 
                                                  sizeof(ipaddr)) != NBD_SUCCESS)
                                {
                                        ipaddr[0] = '\0';
                                }
                                
                                nbu_string_printf( buffer, sizeof buffer, "wlan.clients.%d.ip_addr", n );
                                api_set_value( buffer, ipaddr );
                        }
                        
                        ezxml_free(xmln_root);
                }
                        
                free(buf_xml);
        }
}


/* SET
   wlan_active
   wlan_channel
   wlan_wl0_ssid
   wlan_wl0_enc
   wlan_wl0_keytype
   wlan_wl0_wpakey
   wlan_wl0_wepkeys_n0
*/
API_REGISTER_METHOD( wlan, enable, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	if(nbd_wlan_enable() != 0)
        {
                api_set_custom_error("201",
                                     "Unable to soft enable wireless");
                return;
        }

        nbd_nv_commit( "user" );
}

API_REGISTER_METHOD( wlan, disable, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	if(nbd_wlan_disable() != 0)
        {
                api_set_custom_error("201",
                                     "Unable to soft disable wireless");
                return;
        }

	nbd_nv_commit( "user" );
}

API_REGISTER_METHOD( wlan, setChannel, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;

	if ( ewf_fastcgi_get_parameter( "channel", &param ) == EWF_SUCCESS ) 
	{
		nbd_nv_set( "wlan_channel", param );
		nbd_nv_commit( "user" );
	}
}

API_REGISTER_METHOD( wlan, setWlanMode, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;
        int ret;

	if ( ewf_fastcgi_get_parameter( "mode", &param ) == EWF_SUCCESS 
             && (ret = cfg_set("wlan_mode", param)) >= cfg_set_success) 
	{
                if(ret == cfg_set_success)
                {
                        nbd_nv_commit( "user" );
                }
	}
        else
        {
                api_set_error(API_ERROR_INVALID_ARG);
        }
}

API_REGISTER_METHOD( wlan, setWl0Ssid, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;

	if ( ewf_fastcgi_get_parameter( "ssid", &param ) == EWF_SUCCESS ) 
	{
		nbd_nv_set( "wlan_wl0_ssid", param );
		nbd_nv_commit( "user" );
	}
}

API_REGISTER_METHOD( wlan, setWl0Enc, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;

	if ( ewf_fastcgi_get_parameter( "enc", &param ) == EWF_SUCCESS ) 
	{
		nbd_nv_set( "wlan_wl0_enc", param );
		nbd_nv_commit( "user" );
	}
}

API_REGISTER_METHOD( wlan, setWl0Keytype, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;

	if ( ewf_fastcgi_get_parameter( "keytype", &param ) == EWF_SUCCESS ) 
	{
		nbd_nv_set( "wlan_wl0_keytype", param );
		nbd_nv_commit( "user" );
	}
}

API_REGISTER_METHOD( wlan, setWl0Wpakey, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;

	if ( ewf_fastcgi_get_parameter( "wpakey", &param ) == EWF_SUCCESS ) 
	{
		nbd_nv_set( "wlan_wl0_wpakey", param );
		nbd_nv_commit( "user" );
	}
}

API_REGISTER_METHOD( wlan, setWl0Wepkey, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	char *param = NULL;

	if( ewf_fastcgi_get_parameter( "wepkey", &param ) == EWF_SUCCESS )
	{
		nbd_nv_set( "wlan_wl0_wepkeys_n0", param );
		nbd_nv_commit( "user" );
	}
}

/* ACTION
 * start
 */
API_REGISTER_METHOD( wlan, start, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	nbd_wlan_start(  );
}

/* ACTION
 * stop
 */
API_REGISTER_METHOD( wlan, stop, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
        nbd_sys_delay_run("1", "/usr/bin/wlan", "stop");
}

/* ACTION
 * restart
 */
API_REGISTER_METHOD( wlan, restart, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE )
{
	if ( nbd_status_matches("ses_status", "up") ) {
                /* first trick: suppose we are in a ses context
                 */
                /* second trick: restart wlan 2 seconds after because API client
                 * needs to get response over the wireless radio.
                 */
		nbd_sys_delay_run("1", "/usr/bin/wlan", "ses_done");
		return;
	}

        nbd_sys_delay_run("1", "/usr/bin/wlan", "restart");
}
