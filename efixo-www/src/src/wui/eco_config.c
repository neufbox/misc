#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

PAGE_DF("/eco/config", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        NV_TO_HDF("eco_profile");
        NV_TO_HDF("eco_voip_active");
        NV_TO_HDF("eco_cpl_active");
        NV_TO_HDF("eco_link_lan1_active");
        NV_TO_HDF("eco_link_lan2_active");
        NV_TO_HDF("eco_link_lan3_active");
        NV_TO_HDF("eco_link_lan4_active");
        NV_TO_HDF("eco_link_ftth_active");
        NV_TO_HDF("eco_link_lan1_eee");
        NV_TO_HDF("eco_link_lan2_eee");
        NV_TO_HDF("eco_link_lan3_eee");
        NV_TO_HDF("eco_link_lan4_eee");
        NV_TO_HDF("eco_link_ftth_eee");
        NV_TO_HDF("eco_led_active");

        return;
}

FORM_HANDLER
{
        const char *eco_profile = NULL,
                *eco_voip_active = NULL,
                *eco_cpl_active = NULL,
                *eco_link_lan1_active = NULL,
                *eco_link_lan2_active = NULL,
                *eco_link_lan3_active = NULL,
                *eco_link_lan4_active = NULL,
                *eco_link_ftth_active = NULL,
                *eco_link_lan1_eee = NULL,
                *eco_link_lan2_eee = NULL,
                *eco_link_lan3_eee = NULL,
                *eco_link_lan4_eee = NULL,
                *eco_link_ftth_eee = NULL,
                *eco_led_active = NULL;
        struct trx trx;

        EWF_HDF_GET(eco_profile);
        EWF_HDF_GET_DEFAULT(eco_voip_active, "on");
        EWF_HDF_GET_DEFAULT(eco_cpl_active, "on");
        EWF_HDF_GET(eco_link_lan1_active);
        EWF_HDF_GET(eco_link_lan2_active);
        EWF_HDF_GET(eco_link_lan3_active);
        EWF_HDF_GET(eco_link_lan4_active);
        EWF_HDF_GET(eco_link_ftth_active);

        EWF_HDF_GET_DEFAULT(eco_link_lan1_eee, "off");
        EWF_HDF_GET_DEFAULT(eco_link_lan2_eee, "off");
        EWF_HDF_GET_DEFAULT(eco_link_lan3_eee, "off");
        EWF_HDF_GET_DEFAULT(eco_link_lan4_eee, "off");
        EWF_HDF_GET_DEFAULT(eco_link_ftth_eee, "off");

        EWF_HDF_GET(eco_led_active);

        NV_TRX_INIT(trx);

        NV_SET_WTRX(trx, "eco_profile", eco_profile);
        NV_SET_WTRX(trx, "eco_voip_active", eco_voip_active);
        NV_SET_WTRX(trx, "eco_cpl_active", eco_cpl_active);
        NV_SET_WTRX(trx, "eco_link_lan1_active", eco_link_lan1_active);
        NV_SET_WTRX(trx, "eco_link_lan2_active", eco_link_lan2_active);
        NV_SET_WTRX(trx, "eco_link_lan3_active", eco_link_lan3_active);
        NV_SET_WTRX(trx, "eco_link_lan4_active", eco_link_lan4_active);
        NV_SET_WTRX(trx, "eco_link_ftth_active", eco_link_ftth_active);
        NV_SET_WTRX(trx, "eco_link_lan1_eee", eco_link_lan1_eee);
        NV_SET_WTRX(trx, "eco_link_lan2_eee", eco_link_lan2_eee);
        NV_SET_WTRX(trx, "eco_link_lan3_eee", eco_link_lan3_eee);
        NV_SET_WTRX(trx, "eco_link_lan4_eee", eco_link_lan4_eee);
        NV_SET_WTRX(trx, "eco_link_ftth_eee", eco_link_ftth_eee);
        NV_SET_WTRX(trx, "eco_led_active", eco_led_active);

        if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
        {
                nbu_log_debug("call nbd_eco_update function");
                nbd_eco_update();
        }

        return;
}
