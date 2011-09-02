#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbl_utils.h"
#include "nbl_voip.h"
#include "nbl_tv.h"
#include "nbl_net.h"
#include "nbd.h"

#if defined(HAVE_LED_SUPPORT)
#include "nbl_leds.h"
#endif /* defined(HAVE_LED_SUPPORT) */

PAGE_DFA_u("/state", EWF_ACCESS_PRIVATE);

DISPLAY_AJAX_HANDLER
{
#if defined(HAVE_LED_SUPPORT)
	const char *led_brightness = nbl_leds_brightness_cstr();
#endif /* defined(HAVE_LED_SUPPORT) */

	int timestamp;
	nbl_utils_ltime_t ltm;

        /* access used */
        if(nbd_status_get("net_data_access", buffer, sizeof(buffer)) == NBU_SUCCESS)
        {
                ewf_hdf_set("internet_access", buffer);
        }

        if(nbd_status_get("net_voip_access", buffer, sizeof(buffer)) == NBU_SUCCESS)
        {
                ewf_hdf_set("voip_access", buffer);
        }

	/* Status - wan/voip/tv */
	if (nbd_status_get("net_data_status", buffer, sizeof(buffer)) == NBU_SUCCESS)
	{
		ewf_hdf_set("internet_status", buffer);
	}

        if(nbd_status_get("voip_up", buffer, sizeof(buffer)) == NBU_SUCCESS)
	{
		ewf_hdf_set("voip_status", buffer);
	}

	if (nbl_tv_get_status(buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("tv_status", buffer);
	}

	/* Infos */
	if (nbd_status_get("mac_addr", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("modem_macaddr", buffer);
	}

	if (nbd_status_get("productID", buffer, sizeof buffer) == NBD_SUCCESS)
	{
		ewf_hdf_set("modem_model", buffer);
	}

#ifndef NB5
	if (nbd_status_get("version_dsldriver", buffer, sizeof buffer) == NBD_SUCCESS)
	{
		ewf_hdf_set("modem_adslversion", buffer);
	}
#endif

	if (nbd_status_get("version_mainfirmware", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("modem_mainversion", buffer);
	}

#ifndef CIBOX
	if (nbd_status_get("version_rescuefirmware", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("modem_rescueversion", buffer);
	}
#endif /* #ifndef CIBOX */

	if (nbd_status_get("uptime", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		errno = 0;
		timestamp = strtol(buffer, NULL, 10);
		if(errno == 0)
		{
			nbl_utils_timestamp2ltime(timestamp, &ltm);

			ewf_hdf_set_int("modem_uptime.sec", ltm.sec);
			ewf_hdf_set_int("modem_uptime.min", ltm.min);
			ewf_hdf_set_int("modem_uptime.hour", ltm.hour);
			ewf_hdf_set_int("modem_uptime.day", ltm.day);
		}
	}

#if defined(HAVE_LED_SUPPORT)
	ewf_hdf_set("led_dsl",     nbl_leds_state_cstr(nbd_leds_state(led_id_wan)));
	ewf_hdf_set("led_traffic", nbl_leds_state_cstr(nbd_leds_state(led_id_traffic)));
	ewf_hdf_set("led_tel",     nbl_leds_state_cstr(nbd_leds_state(led_id_voip)));
	ewf_hdf_set("led_tv",      nbl_leds_state_cstr(nbd_leds_state(led_id_tv)));
	ewf_hdf_set("led_wifi",    nbl_leds_state_cstr(nbd_leds_state(led_id_wlan)));
	ewf_hdf_set("led_alarm",   nbl_leds_state_cstr(nbd_leds_state(led_id_alarm)));

	nbu_log_debug("led_brightness = %s", led_brightness);

	ewf_hdf_set("leds_state", led_brightness);
#endif /* defined(HAVE_LED_SUPPORT) */

        STATUS_TO_HDF("ipv6_status");
        STATUS_TO_HDF("ipv6_lock");
        NV_TO_HDF("ipv6_enable");

	return;
}

FORM_HANDLER
{
#if defined(HAVE_LED_SUPPORT)
	int restart = 0;
	enum led_state brightness_action;

	if(ewf_hdf_sget_copy("leds_state",  buffer, sizeof  buffer) == NBU_SUCCESS)
	{
		if (strcmp(buffer, "on") == 0) {
			brightness_action = led_state_on;
		}
		else
		{
			brightness_action = led_state_off;
		}


		if (nbd_user_config_set("led_brightness", nbl_leds_state_cstr(brightness_action)) == NBD_SUCCESS)
		{
			restart++;
		}

		if (restart > 0)
		{
			(void) nbd_leds_brightness(brightness_action);

			/* wait some microsecond to allow time for the led state modification */
			usleep(250);
		}
	}

	if( restart > 0 )
	{
		nbd_user_config_commit();
	}
#endif /* defined(HAVE_LED_SUPPORT) */

	return;
}

