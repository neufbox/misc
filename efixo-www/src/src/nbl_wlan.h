#ifndef _NBL_WLAN_H_
#define _NBL_WLAN_H_

#include "nbu/net.h"

enum wlan_chk_cpty_return {
	wlan_error_11n_legacy_mode_and_wep_invalid = -100,
	wlan_error_11n_only_mode_and_wpa_psk_invalid,
	wlan_error_11n_only_mode_and_wpa_wpa2_psk_invalid,
	wlan_error_11n_only_mode_and_wep_invalid,
	wlan_compatibility_ok = 0,
};

/*
 * check compatibility between wlan mode and the encryption
 *
 * 802.11n => only wpa2
 * 802.11n/b/g => wpa2, wpa, wpa/wpa2
 * 802.11b/g => all
 *
 * Return NBU_SUCCESS if correct
 */
extern int nbl_wlan_check_compatibility(const char *wlan_mode,
					const char *wlan_enc);


#endif
