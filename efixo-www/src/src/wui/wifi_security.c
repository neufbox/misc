#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>

#include "nbl.h"
#include "nbl_wlan.h"
#include "nbd.h"
#include "dbl.h"

PAGE_DF("/wifi/security", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        nv_to_hdf("wlan_wl0_enc", "wlan_encryptiontype");
        nv_to_hdf("wlan_wl0_keytype", "wlan_keytype");
        nv_to_hdf("wlan_wl0_wepkeys_n0", "wlan_wepkey");
        nv_to_hdf("wlan_wl0_enctype", "wlan_wpaenctype");
        nv_to_hdf("wlan_wl0_wpakey", "wlan_wpakey");

        return;
}

FORM_HANDLER
{
        const char *pattern = NULL;
        const char *wlan_encryptiontype = NULL,
                *wlan_keytype = NULL,
                *wlan_wpaenctype = NULL,
                *wlan_wpakey = NULL,
                *wlan_wepkey = NULL;
        char wlan_mode[32];

        struct trx trx;

        NV_GET(wlan_mode);

        EWF_HDF_GET(wlan_encryptiontype);

        /* check compatibility before set things */
        if(nbl_wlan_check_compatibility(wlan_mode, wlan_encryptiontype)
                != wlan_compatibility_ok)
        {
                ewf_hdf_form_set_error("enc_mode_incompatibility");
                return; /* RETURN */
        }

        /* GET and CHECK */
        if(nbu_string_matches(wlan_encryptiontype, "WEP") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(wlan_keytype);

                if(nbu_string_matches(wlan_keytype, "hexa") == NBU_STRING_EQUAL)
                {
                        pattern = ewf_hdf_get_env("Var.wlan_wepkey.Regex.hexa");
                }
                else
                {
                        pattern = ewf_hdf_get_env("Var.wlan_wepkey.Regex.ascii");
                }


                /* check keys are goods */
                wlan_wepkey = ewf_hdf_get("wlan_wepkey");

                if(ewf_utils_pcre_validate(wlan_wepkey, pattern) == NBU_ERROR)
                {
                        ewf_hdf_set_error("wlan_wepkey", wlan_keytype);
                        return; /* RETURN */
                }
        }
        else {
                EWF_HDF_GET(wlan_wpaenctype);
                EWF_HDF_GET(wlan_wpakey);
        }
        
        //force if not javascript
        if(nbu_string_matches(wlan_encryptiontype, "WPA2-PSK") == NBU_STRING_EQUAL)
        {
                wlan_wpaenctype="aes";
        }

        /* trx */
        NV_TRX_INIT(trx);

        NV_SET_WTRX(trx, "wlan_wl0_enc", wlan_encryptiontype);

        if(nbu_string_matches(wlan_encryptiontype, "WEP") == NBU_STRING_EQUAL)
        {
                NV_SET_WTRX(trx, "wlan_wl0_keytype", wlan_keytype);

                if(wlan_wepkey != NULL)
                {
                        NV_SET_WTRX(trx, "wlan_wl0_wepkeys_n0", wlan_wepkey);
                }
        }
        else
        {
                NV_SET_WTRX(trx, "wlan_wl0_wpakey", wlan_wpakey);
                NV_SET_WTRX(trx, "wlan_wl0_enctype", wlan_wpaenctype);
        }

        if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
        {
#ifndef X86
                nbd_wlan_restart();
#endif
        }

        return;
}
