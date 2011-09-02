#include <string.h>
#include <ezxml.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbd.h"
#include "dbl.h"

PAGE_DFA_u("/hotspot/config", EWF_ACCESS_PRIVATE);

DISPLAY_AJAX_HANDLER
{
        if(nbd_autoconf_matches("hotspot_enable", "false"))
        {
                nbu_log_notice("hotspot is disabled.");
                ewf_hdf_set("hotspot_enable", "false");
        }

        status_to_hdf("hotspot_status", "hotspot_status");
        nv_to_hdf("hotspot_active", "hotspot_active");
        nv_to_hdf("hotspot_mode", "hotspot_mode");

        if(nbd_hotspot_ssid(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("hotspot_ssid", buffer);
        }

        return;
}

FORM_HANDLER
{
        const char *hotspot_conditions = NULL;
        const char *hotspot_active = NULL;
        const char *hotspot_mode = NULL;
        struct trx trx;

        if(nbd_autoconf_matches("hotspot_enable", "false"))
        {
                nbu_log_notice("hotspot is disabled. Drop form submit.");
                return;
        }

        EWF_HDF_GET(hotspot_active);

        if (nbu_string_matches(hotspot_active, "on") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET_DEFAULT(hotspot_conditions, "decline");
                EWF_HDF_GET(hotspot_mode);

                if (nbu_string_matches(hotspot_conditions,
                                       "accept") != NBU_STRING_EQUAL)
                {
                        ewf_hdf_set_error("hotspot_conditions", "no_accept");
                        return;
                }

                NV_TRX_INIT(trx);

                NV_SET_WTRX(trx, "hotspot_active", hotspot_active);
                NV_SET_WTRX(trx, "hotspot_mode", hotspot_mode);

                if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
                {
                        if (nbu_string_matches(hotspot_mode,
                                               "sfr_fon")
                            == NBU_STRING_EQUAL)
                        {
                                ewf_hdf_set("hotspot_chosefon", "yes");
                        }

                        /* working status display if net_data_access != gprs*/
                        nbd_status_get("net_data_access", buffer, sizeof(buffer));
                        if (nbu_string_compare(buffer, "gprs") == NBU_STRING_EQUAL)
                        {
                                return ;
                        }
                        ewf_hdf_set("Page.Action", "work");
#ifndef X86
                        nbd_hotspot_restart();
#endif
                }
        }
        else if (nbu_string_matches(hotspot_active, "off") == NBU_STRING_EQUAL)
        {
                NV_TRX_INIT(trx);

                NV_SET_WTRX(trx, "hotspot_active", hotspot_active);

                if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
                {
                        /* working status display */
                        ewf_hdf_set("Page.Action", "work");
#ifndef X86
                        nbd_hotspot_stop();
#endif
                }
        }

        return;
}
