#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "kit_helper.h"

#include "nbd.h"

#include <nbu/nbu.h>
#include <nbu/string.h>

#include <nbh/startstop.h>

/*************************/
/* static defs           */
/*************************/
static struct {
	char *name;
	int (*func_read)(const char *name, char *buf, size_t buf_size);
	int (*func_write)(const char *name, const char *value);
	char *forcedvalue;
        int access_policy;
} request_name_maps[] = {
	{ "adsl_attenuation_down", nbd_dsl_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "adsl_attenuation_up", nbd_dsl_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "adsl_noise_down", nbd_dsl_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "adsl_noise_up", nbd_dsl_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "adsl_rate_down", nbd_dsl_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "adsl_rate_up", nbd_dsl_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "adsl_status", nbd_status_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "filter_simple_smtpdrop", nbd_user_config_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
#ifdef CIBOX
	{ "hotspot_active", NULL, NULL, "off", EWF_ACCESS_PUBLIC, },
	{ "hotspot_mode", NULL, NULL, "sfr", EWF_ACCESS_PUBLIC, },
#else
	{ "hotspot_active", NULL, nbd_user_config_set, NULL, EWF_ACCESS_PUBLIC, },
	{ "hotspot_mode", NULL, nbd_user_config_set, NULL, EWF_ACCESS_PUBLIC, },
#endif
	{ "link_lan4_status", nbd_status_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "mac_addr", nbd_status_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "net_mode", NULL, NULL, "router", EWF_ACCESS_PUBLIC, },
	{ "net_data_ipaddr", nbd_status_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "net_data_status", nbd_status_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "ppp_login", nbd_user_config_get, nbd_user_config_set, NULL, EWF_ACCESS_PUBLIC, },
	{ "ppp_password", nbd_user_config_get, nbd_user_config_set, NULL, EWF_ACCESS_PUBLIC, },
	{ "version_mainfirmware", nbd_status_get, NULL, NULL, EWF_ACCESS_PUBLIC, },
	{ "wlan_wl0_enc", nbd_user_config_get, nbd_user_config_set, NULL, EWF_ACCESS_PRIVATE, },
	{ "wlan_wl0_keytype", NULL, nbd_user_config_set, NULL, EWF_ACCESS_PRIVATE, },
        { "wlan_wl0_primarykey", NULL, NULL, "0", EWF_ACCESS_PRIVATE, }, /* to ensure retrocompatibility with old kit */
	{ "wlan_wl0_ssid", nbd_user_config_get, nbd_user_config_set, NULL, EWF_ACCESS_PRIVATE, },
	{ "wlan_wl0_wepkeys_n0", nbd_user_config_get, nbd_user_config_set, NULL, EWF_ACCESS_PRIVATE, },
	{ "wlan_wl0_wpakey", nbd_user_config_get, nbd_user_config_set, NULL, EWF_ACCESS_PRIVATE, },
	{ "wpa_key", nbd_status_get, NULL, NULL, EWF_ACCESS_PRIVATE, },
};

static struct {
	char *oldname;
	char *newname;
} old_vars_compatibility_maps[] = {
	{ "chilli_active", "hotspot_active", },
	{ "chilli_mode", "hotspot_mode", },
	{ "link_adsl_status", "adsl_status", },
	{ "ppp_ipaddr", "net_data_ipaddr", },
	{ "ppp_status", "net_data_status", },
};

#define ACTION_COMMIT 0
#define ACTION_INITSCRIPT 1
#define ACTION_FUNC 2

static struct {
	char *scriptname;
	char *actioname;
	short int typeaction;
	int (*func_action)( void );
} script_action_maps[] = {
#ifndef CIBOX
	{ "hotspot", "stop", ACTION_FUNC, nbd_hotspot_stop, },
	{ "hotspot", "restart", ACTION_FUNC, nbd_hotspot_restart, },
	{ "hotspot", "commit", ACTION_COMMIT, NULL, },
#endif
	{ "net_mode", "restart", ACTION_INITSCRIPT, NULL, }, /* no restart action exist */
	{ "ppp", "restart", ACTION_INITSCRIPT, NULL, },
	{ "wlan", "ses_stop", ACTION_FUNC, nbd_wlan_ses_done, },
	{ "wlan", "restart", ACTION_FUNC, nbd_wlan_restart, },
	{ "wlan", "commit", ACTION_COMMIT, NULL, },
};

static struct {
	char *oldscriptname;
	char *oldaction;
	char *newname;
	char *newaction;
} old_script_action_compatibility_maps[] = {
	{ "chillispot", NULL, "hotspot", NULL, }, /* for all actions */
	{ "ses", "stop", "wlan", "ses_stop", },
};

/*************************/
/* static function       */
/*************************/

static const char * _kit_helper_var_compatibility(const char *name)
{
	unsigned int i;

	for(i = 0; i < NBU_ARRAY_SIZE(old_vars_compatibility_maps); i++)
	{
		if(nbu_string_compare(name, old_vars_compatibility_maps[i].oldname)
		   == NBU_STRING_EQUAL)
		{
			name = old_vars_compatibility_maps[i].newname;

			break;
		}
	}

	return name;
}

/*************************/
/* public function       */
/*************************/

int kit_helper_get(const char *name, char *buf, size_t buf_size)
{
	unsigned int i;
	int ret = -1;

	name = _kit_helper_var_compatibility(name);

	nbu_log_debug(" * try to GET %s value",
		      name);

	for(i = 0; i < NBU_ARRAY_SIZE(request_name_maps); i++)
	{
		if(nbu_string_compare(name, request_name_maps[i].name)
                   == NBU_STRING_EQUAL)
		{
                        if(request_name_maps[i].access_policy == EWF_ACCESS_PRIVATE
                           && !nbd_status_matches("web_autologin", "on"))
                        {
                                nbu_log_error("GET of '%s' failed"
                                              " (access denied, reason: auth button down)",
                                              name);
                                ret = -1;
                                break;
                        }

			if(request_name_maps[i].forcedvalue != NULL)
			{
				nbu_log_debug(" * GET SUCCESS %s (forced value)",
					      request_name_maps[i].forcedvalue);

				nbu_string_printf(buf,
						  buf_size,
						  "%s",
						  request_name_maps[i].forcedvalue);
				ret = 0;
			}
			else if(request_name_maps[i].func_read != NULL)
			{
				ret = request_name_maps[i].func_read(name,
								     buf,
								     buf_size);
#ifdef DEBUG
				if(ret == 0)
				{
					nbu_log_debug(" * GET SUCCESS %s (fresh value)",
						      buf);
				}
				else
				{
					nbu_log_debug(" * GET FAILED");
				}
#endif
			}

			break;
		}
	}

	return ret;
}

int kit_helper_set(const char *name, const char *value)
{
	unsigned int i;
	int ret = -1;

 	name = _kit_helper_var_compatibility(name);

	nbu_log_debug("SET %s = %s",
		      name,
		      value);

	for(i = 0; i < NBU_ARRAY_SIZE(request_name_maps); i++)
	{
		if(nbu_string_compare(name, request_name_maps[i].name)
		   == NBU_STRING_EQUAL)
		{
                        if(request_name_maps[i].access_policy == EWF_ACCESS_PRIVATE
                           && !nbd_status_matches("web_autologin", "on"))
                        {
                                nbu_log_error("SET '%s' = '%s' failed"
                                              " (access denied, reason: auth button down)",
                                              name, value);
                                ret = -1;
                                break;
                        }

			if(request_name_maps[i].func_write != NULL)
			{
				nbu_log_debug("CORRECT REQUEST, SET IT");
				ret = request_name_maps[i].func_write(name,
								      value);
			}

			break;
		}
	}

	return ret;
}

int kit_helper_action(const char *scriptname, const char *action)
{
	int ret = -1;
	unsigned int i;
        char buf[256];

	/* compatibility for old kit */
	for(i = 0; i < NBU_ARRAY_SIZE(old_script_action_compatibility_maps); i++)
	{
		if(nbu_string_compare(scriptname,
				      old_script_action_compatibility_maps[i].oldscriptname)
		   == NBU_STRING_EQUAL)
		{
			if(old_script_action_compatibility_maps[i].oldaction == NULL)
			{
				scriptname = old_script_action_compatibility_maps[i].newname;
				break;
			}
			else if(nbu_string_compare(action,
						   old_script_action_compatibility_maps[i].oldaction)
				== NBU_STRING_EQUAL)
			{
				scriptname = old_script_action_compatibility_maps[i].newname;
				action = old_script_action_compatibility_maps[i].newaction;
			}
		}
	}

	for(i = 0; i < NBU_ARRAY_SIZE(script_action_maps); i++)
	{
		if(nbu_string_compare(scriptname,
				      script_action_maps[i].scriptname)
		   == NBU_STRING_EQUAL
		   && nbu_string_compare(action,
					 script_action_maps[i].actioname)
		   == NBU_STRING_EQUAL)
		{
			if(script_action_maps[i].typeaction == ACTION_FUNC)
			{
				nbu_log_debug("%s %s (FUNC ACTION)",
					      scriptname,
					      action);
				ret = script_action_maps[i].func_action();
			}
			else if(script_action_maps[i].typeaction == ACTION_INITSCRIPT)
			{
                                nbu_string_printf(buf, sizeof(buf),
                                                  "/etc/init.d/%s",
                                                  script_action_maps[i].scriptname);

				nbu_log_debug("%s %s (SCRIPT ACTION)",
					      buf,
					      action);
				ret = nbd_sys_async_run(buf,
							script_action_maps[i].actioname);

			}
			else if(script_action_maps[i].typeaction == ACTION_COMMIT)
			{
				nbu_log_debug("%s %s (COMMIT ACTION)",
					      scriptname,
					      action);
				nbd_user_config_commit();
				ret = 0;
			}
			else
			{
				nbu_log_debug("%s %s (!! ERROR !!)",
					      scriptname,
					      action);
				nbu_log_error("Invalid action type '%d'",
					      script_action_maps[i].typeaction);
			}

			break;
		}
	}

	return ret;
}

#ifdef DEBUG
PAGE_DF("/kit/test", EWF_ACCESS_PUBLIC);

DISPLAY_HANDLER
{

}

FORM_HANDLER
{
	unsigned int i;
	int ret;
	char buf[256];

	nbu_log_debug(" *** test get/set old compatibility *** ");
	for(i = 0; i < NBU_ARRAY_SIZE(old_vars_compatibility_maps); i++)
	{
		sleep(1);
		nbu_log_debug(" ------------------------------------ ");
		nbu_log_debug(" ----- TEST %s",
			      old_vars_compatibility_maps[i].oldname);

		if(kit_helper_get(old_vars_compatibility_maps[i].oldname,
				  buf,
				  sizeof(buf)) == 0)
		{
			nbu_log_debug(" - GET OK ( %s )",
				      buf);
		}
		else
		{
			nbu_log_debug(" - GET FAILED or UNAVAILABLE ...");

			continue;
		}

		if((ret = kit_helper_set(old_vars_compatibility_maps[i].oldname,
					 buf)) != -1)
		{
			nbu_log_debug(" - SET OK ( %s )",
				      ret == 0 ? "SUCCESS" : "NOCHG");
		}
		else
		{
			nbu_log_debug(" - SET FAILED or UNAVAILABLE ...");
		}
	}

	nbu_log_debug(" *** test get/set *** ");
	for(i = 0; i < NBU_ARRAY_SIZE(request_name_maps); i++)
	{
		sleep(1);
		nbu_log_debug(" ------------------------------------ ");
		nbu_log_debug(" ----- TEST %s",
			      request_name_maps[i].name);

		if(kit_helper_get(request_name_maps[i].name,
				  buf,
				  sizeof(buf)) == 0)
		{
			nbu_log_debug(" - GET OK ( %s )",
				      buf);
		}
		else
		{
			if(request_name_maps[i].func_read == NULL)
			{
				nbu_log_debug(" - GET INVALID (OK !)");
			}
			else
			{
				nbu_log_debug(" - GET FAILED");
			}

			continue;
		}

		if((ret = kit_helper_set(request_name_maps[i].name,
					 buf)) != -1)
		{
			nbu_log_debug(" - SET OK ( %s )",
				      ret == 0 ? "SUCCESS" : "NOCHG");
		}
		else
		{
			if(request_name_maps[i].func_write == NULL)
			{
				nbu_log_debug(" - SET INVALID (OK !)");
			}
			else
			{
				nbu_log_debug(" - SET FAILED");
			}
		}
	}

	nbu_log_debug(" *** test script action old compatibility *** ");
	for(i = 0; i < NBU_ARRAY_SIZE(old_script_action_compatibility_maps); i++)
	{
		sleep(1);
		nbu_log_debug(" ------------------------------------ ");
		nbu_log_debug(" ----- TEST %s %s",
			      old_script_action_compatibility_maps[i].oldscriptname,
			      old_script_action_compatibility_maps[i].oldaction);

		if(old_script_action_compatibility_maps[i].oldaction != NULL)
		{
			kit_helper_action(old_script_action_compatibility_maps[i].oldscriptname,
					  old_script_action_compatibility_maps[i].oldaction);
		}
		else
		{
			kit_helper_action(old_script_action_compatibility_maps[i].oldscriptname,
					  "restart");
		}

	}

	nbu_log_debug(" *** test script action *** ");
	for(i = 0; i < NBU_ARRAY_SIZE(script_action_maps); i++)
	{
		sleep(1);
		nbu_log_debug(" ------------------------------------ ");
		nbu_log_debug(" ----- TEST %s %s",
			      script_action_maps[i].scriptname,
			      script_action_maps[i].actioname);

		kit_helper_action(script_action_maps[i].scriptname,
				  script_action_maps[i].actioname);
	}
}
#endif
