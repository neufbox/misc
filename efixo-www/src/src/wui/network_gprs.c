#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbd.h"

#include "dbl.h"

#include "nbd/backup3g.h"

PAGE_DFA_u("/network/gprs", EWF_ACCESS_PRIVATE);

DISPLAY_AJAX_HANDLER
{
        status_to_hdf("gprs_active_data", "gprs_active_data");
        status_to_hdf("gprs_device_state", "gprs_device_state");
        status_to_hdf("gprs_sim_status", "gprs_sim_status");
        status_to_hdf("gprs_sim_pinstatus", "gprs_sim_pinstatus");
        status_to_hdf("gprs_sim_pinremaining", "gprs_sim_pinremaining");
        status_to_hdf("gprs_sim_pukremaining", "gprs_sim_pukremaining");

        nv_to_hdf("gprs_service_data", "gprs_service_data");
        nv_to_hdf("gprs_service_voice", "gprs_service_voice");
        nv_to_hdf("gprs_pin", "gprs_pin");
        nv_to_hdf("gprs_apn", "gprs_apn");
}

FORM_HANDLER
{
        int ret;
        const char *action = NULL,
                *gprs_service_data = NULL,*gprs_service_voice = NULL, *gprs_pin = NULL, *gprs_apn = NULL,
                *gprs_pukcode = NULL, *gprs_newpincode = NULL;
        struct trx trx;

        EWF_HDF_GET(action);

        if(nbu_string_matches(action, "editcfg") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(gprs_service_data);
                EWF_HDF_GET(gprs_service_voice);
                EWF_HDF_GET(gprs_pin);
                EWF_HDF_GET(gprs_apn);

                NV_TRX_INIT(trx);

                NV_SET_WTRX(trx, "gprs_service_data", gprs_service_data);
                NV_SET_WTRX(trx, "gprs_service_voice", gprs_service_voice);
                NV_SET_WTRX(trx, "gprs_pin", gprs_pin);
                NV_SET_WTRX(trx, "gprs_apn", gprs_apn);

                if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
                {
                        /* reload backup3g */
                        nbd_sys_sync_run(BACKUP3G_START_STOP, "reload");
                }
        }
        else if(nbu_string_matches(action, "unlocksim") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(gprs_pukcode);
                EWF_HDF_GET(gprs_newpincode);

                ret = nbd_backup3g_puk_unlock(gprs_pukcode, gprs_newpincode);
                if(ret == NBD_SUCCESS)
                {
                        nbu_log_debug("puk unlock OK ! sim card is unlocked");

                        nbd_nv_commit("user");

                        /* restart backup3g */
                        nbd_sys_sync_run(BACKUP3G_START_STOP, "reload");
                }
                else if(ret == backup3g_error_invalid_puk)
                {
                        ewf_hdf_query_var_error("gprs_pukcode", "invalid");
                }
                else if(ret == backup3g_error_invalid_newpin)
                {
                        ewf_hdf_query_var_error("gprs_newpincode", "invalid");
                }
                else
                {
                        ewf_hdf_query_error("unknown");
                }
        }
}
