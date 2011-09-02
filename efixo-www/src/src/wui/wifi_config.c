#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbl_wlan.h"
#include "nbd.h"
#include "dbl.h"

PAGE_DF("/wifi/config", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        if(nbd_wlan_active(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("ap_active", buffer);
        }

        if(nbd_status_get("wlan_wlan0_cap_interrupter",
                          buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("wlan_cap.wlan0.interrupter", buffer);
        }

        nv_to_hdf("wlan_wl0_ssid", "ap_ssid");
        nv_to_hdf("wlan_closed", "ap_closed");

        if (nbd_user_config_get("wlan_autochannel", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                if(strncmp(buffer, "on", 2) == 0)
                {
                        ewf_hdf_set("ap_channel", "auto");
                }
                else
                {
                        nv_to_hdf("wlan_channel", "ap_channel");
                }
        }

        nv_to_hdf("wlan_mode", "ap_mode");

#if defined(NB5) || defined(NB6)
        nv_to_hdf("wlan_ht40", "ap_ht40");
#endif

        return;
}

FORM_HANDLER
{
        const char *ap_active = NULL,
                *ap_mode = NULL,
                *ap_ssid = NULL,
                *ap_closed = NULL,
                *ap_channel = NULL;
#if defined(NB5) || defined(NB6)
        const char *ap_ht40 = NULL;
#endif
        unsigned short int restart = 0;
        struct trx trx;
        char wlan_wl0_enc[64];

        NV_GET(wlan_wl0_enc);

        EWF_HDF_GET(ap_active);
        EWF_HDF_GET(ap_mode);
        EWF_HDF_GET(ap_ssid);
        EWF_HDF_GET(ap_closed);
        EWF_HDF_GET(ap_channel);
#if defined(NB5) || defined(NB6)
        EWF_HDF_GET_DEFAULT(ap_ht40, "off");
#endif
        /* check compatibility before set things */
        if(nbl_wlan_check_compatibility(ap_mode,
                                        wlan_wl0_enc) != wlan_compatibility_ok)
        {
                ewf_hdf_form_set_error("enc_mode_incompatibility");
                return; /* RETURN */
        }

        if (nbu_string_matches(ap_active, "off") == NBU_STRING_EQUAL)
        {
#ifndef X86
                nbd_wlan_stop();
#endif
                /* must be done AFTER wlan stop */
                nbd_wlan_disable();
                nbd_user_config_commit();
                restart++;
        }
        else if (nbu_string_matches(ap_active, "on") == NBU_STRING_EQUAL)
        {
                if (nbd_wlan_enable() == NBD_SUCCESS)
                {
                        nbd_user_config_commit();
                        restart++;
                }
        }

        NV_TRX_INIT(trx);
        NV_SET_WTRX(trx, "wlan_wl0_ssid", ap_ssid);
        NV_SET_WTRX(trx, "wlan_closed", ap_closed);

        if(nbu_string_matches(ap_channel, "auto") == NBU_STRING_EQUAL)
        {
                NV_SET_WTRX(trx, "wlan_autochannel", "on");
        }
        else
        {
                NV_SET_WTRX(trx, "wlan_autochannel", "off");
                NV_SET_WTRX(trx, "wlan_channel", ap_channel);
        }

        /* mode */
        NV_SET_WTRX(trx, "wlan_mode", ap_mode);

#if defined(NB5) || defined(NB6)
        /* ht40 */
        NV_SET_WTRX(trx, "wlan_ht40", ap_ht40);
#endif

        if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
        {
                restart++;
        }

        if( restart > 0) {
#ifndef X86
                nbd_wlan_restart();
#endif
        }
        return;
}
