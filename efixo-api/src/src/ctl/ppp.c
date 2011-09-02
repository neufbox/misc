#include "api_common.h"

/* GET
 * ppp_login
 * ppp_password
 * ppp_status
 * ppp_ipaddr
 */
API_REGISTER_METHOD(ppp, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC)
{
        char data_infra[64];
        char name[128];

        if(nbd_status_get("net_data_access", data_infra, sizeof(data_infra)) == NBD_SUCCESS)
        {
                nbu_string_printf(name, sizeof(name), "ppp_%s_status", data_infra);
                if (nbd_status_get(name, buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        api_set_value("ppp.status", buffer);
                }

                nbu_string_printf(name, sizeof(name), "ppp_%s_ipaddr", data_infra);
                if (nbd_status_get(name, buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        api_set_value("ppp.ip_addr", buffer);
                }
        }
}

API_REGISTER_METHOD(ppp, getCredentials, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE)
{
	if (nbd_nv_get("ppp_login", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("ppp.login", buffer);
	}

	if (nbd_nv_get("ppp_password", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		api_set_value("ppp.password", buffer);
	}
}

/* SET
 * ppp_login
 * ppp_password
 */
API_REGISTER_METHOD(ppp, setCredentials, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
	char *param = NULL;
	int restart_ppp = 0;

	if (ewf_fastcgi_get_parameter("login", &param) == EWF_SUCCESS)
	{
		if (nbd_nv_set("ppp_login", param) == NBD_SUCCESS)
		{
			restart_ppp++;
		}
	}

	if (ewf_fastcgi_get_parameter("password", &param) == EWF_SUCCESS)
	{
		if (nbd_nv_set("ppp_password", param) == NBD_SUCCESS)
		{
			restart_ppp++;
		}
	}

	if (restart_ppp > 0)
	{
		nbd_nv_commit("user");
		nbd_sys_async_run(PPP_START_STOP, "reload");
	}
}
