#include "nbl.h"
#include "nbl_wlan.h"

#include "nbd.h"
#include "nbd/wlan.h"

#include "nbu/nbu.h"
#include "nbu/net.h"
#include "nbu/log.h"
#include "nbu/string.h"

#include <net/if.h>

/*
 * - 11g (11b/g) => all enc and enctype allowed
 * - 11ng (11b/g/n) => no 'WEP' enc allowed
 * - 11n (11n) => only 'WPA2-PSK' or 'OPEN' enc is allowed
 */
int nbl_wlan_check_compatibility(const char *wlan_mode,
				 const char *wlan_enc)
{
	int ret = wlan_compatibility_ok;

	if(nbu_string_matches(wlan_mode, "11g") == NBU_STRING_EQUAL)
	{
		ret = wlan_compatibility_ok;
	}
	else if(nbu_string_matches(wlan_mode, "11ng") == NBU_STRING_EQUAL)
	{
		if(!nbu_string_matches(wlan_enc, "WEP") == NBU_STRING_EQUAL)
		{
			ret = wlan_compatibility_ok;
		}
		else
		{
			ret = wlan_error_11n_legacy_mode_and_wep_invalid;
		}
	}
	else if(nbu_string_matches(wlan_mode, "11n") == NBU_STRING_EQUAL)
	{
		if(nbu_string_matches(wlan_enc, "WEP") == NBU_STRING_EQUAL)
		{
			ret = wlan_error_11n_only_mode_and_wep_invalid;
		}
		else if(nbu_string_matches(wlan_enc, "WPA-PSK") == NBU_STRING_EQUAL)
		{
			ret = wlan_error_11n_only_mode_and_wpa_psk_invalid;
		}
		else if(nbu_string_matches(wlan_enc, "WPA-WPA2-PSK") == NBU_STRING_EQUAL)
		{
			ret = wlan_error_11n_only_mode_and_wpa_wpa2_psk_invalid;
		}
		else
		{
			ret = wlan_compatibility_ok;
		}
	}
	else
	{
		nbu_log_notice("No compatibility map for wlan_mode '%s' and "
                               "wlan_enc '%s'. Guess is ok !",
                               wlan_mode, wlan_enc);
	}

	return ret;
}
