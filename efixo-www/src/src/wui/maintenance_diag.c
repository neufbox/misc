#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>
#include <ewf/core/ewf_renderer.h>

#include "webapp.h"

#include "nbl.h"
#include "nbl_exim.h"
#include "nbd.h"

#include "nbd/vSStest.h"
#include "nbd/spy.h"

#include <ezxml.h>

PAGE_DFA("/maintenance/diag", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        ewf_hdf_set("vSStest_nextstep", "init");

        return;
}

FORM_HANDLER
{
        const char *action = NULL, *step = NULL;
        vSS_testResults ptestResults;

#ifndef NB5
        char *buf_xml = NULL;
        size_t buf_xml_size;
        ezxml_t xmln_root, xmln_result;
#endif

        action = ewf_hdf_sget("action");
        if(action == NULL)
        {
                return;
        }

        if(nbu_string_matches(action,
                              "diagreport_export") == NBU_STRING_EQUAL)
        {
                ewf_renderer_force_raw_output();
                nbl_exim_diagnostic_report_export();
        }
        else if(nbu_string_matches(action, "vSStest") == NBU_STRING_EQUAL)
        {
                step = ewf_hdf_sget("step");
                if(step == NULL)
                {
                        return;
                }

                ewf_hdf_set("vSStest_step", step);

                if(nbu_string_matches(step,
                                      "init") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_initScript();
                        (void) nbd_vSStest_init();

                        do
                        {
                                nbu_log_debug("init status = %d",
                                              ptestResults.init_status);

                                usleep(300000); /* 300ms between poll */
                                nbd_vSStest_getResult(&ptestResults);
                        } while(ptestResults.init_status ==
                                VSS_TEST_IN_PROGRESS);

                        if(ptestResults.init_status == VSS_TEST_SUCCEEDED)
                        {
                                ewf_hdf_set("vSStest_nextstep", "gr909");
                        }
                        else
                        {
                                ewf_hdf_set("vSStest_nextstep", "init");
                        }
                }
                else if(nbu_string_matches(step,
                                           "gr909") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_gr909();

                        nbu_log_debug("ptestResults.gr909_status = %d",
                                      ptestResults.gr909_status);

                        do
                        {
                                nbu_log_debug("Gr909 status = %d",
                                              ptestResults.gr909_status);
                                usleep(300000); /* 300ms between poll */
                                nbd_vSStest_getResult(&ptestResults);
                        } while(ptestResults.gr909_status ==
                                VSS_TEST_IN_PROGRESS);

                        if(ptestResults.gr909_status == VSS_TEST_SUCCEEDED)
                        {
                                ewf_hdf_set("vSStest_nextstep", "ring");
                        }
                        else
                        {
                                ewf_hdf_set("vSStest_nextstep", "gr909");
                        }
                }
                else if(nbu_string_matches(step,
                                           "ring") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_ring();

                        do
                        {
                                nbu_log_debug("Ring status = %d",
                                              ptestResults.ring_status);

                                usleep(300000); /* 300ms between poll */
                                nbd_vSStest_getResult(&ptestResults);
                        } while(ptestResults.ring_status ==
                                VSS_TEST_IN_PROGRESS);

                        if(ptestResults.ring_status == VSS_TEST_SUCCEEDED)
                        {
                                ewf_hdf_set("vSStest_nextstep", "hookntone");
                        }
                        else
                        {
                                ewf_hdf_set("vSStest_nextstep", "ring");
                        }
                }
                else if(nbu_string_matches(step,
                                           "hookntone") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_hookntone();

                        do
                        {
                                nbu_log_debug("Hookntone status = %d",
                                              ptestResults.hookntone_status);

                                usleep(300000); /* 300ms between poll */
                                nbd_vSStest_getResult(&ptestResults);
                        } while(ptestResults.hookntone_status ==
                                VSS_TEST_IN_PROGRESS);

                        if(ptestResults.hookntone_status == VSS_TEST_SUCCEEDED)
                        {
                                ewf_hdf_set("vSStest_nextstep", "dtmf");
                        }
                        else
                        {
                                ewf_hdf_set("vSStest_nextstep", "hookntone");
                        }
                }
                else if(nbu_string_matches(step,
                                           "dtmf") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_dtmf();

                        do
                        {
                                nbu_log_debug("Dtmf status = %d",
                                              ptestResults.dtmf_status);

                                usleep(300000); /* 300ms between poll */
                                nbd_vSStest_getResult(&ptestResults);
                        } while(ptestResults.dtmf_status ==
                                VSS_TEST_IN_PROGRESS);

                        if(ptestResults.dtmf_status == VSS_TEST_SUCCEEDED)
                        {
                                ewf_hdf_set("vSStest_nextstep", "");
                        }
                        else
                        {
                                ewf_hdf_set("vSStest_nextstep", "dtmf");
                        }
                }
                else
                {
                        nbu_log_error("No step '%s' for vSStest", step);
                        return; /* RETURN */
                }
                /* Spy send event that we have new results on voice test */
                nbd_spy_event("voice-test");

                /* analyze the result */

                /* init */
                if(ptestResults.init_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("InitResult", "not_launched");
                }
                else if(ptestResults.init_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("InitResult", "success");
                }
                else if(ptestResults.init_status == VSS_TEST_FAILED_CHECK)
                {
                        ewf_hdf_set("InitResult", "error");
                        ewf_hdf_set("InitError", "check_hook");
                }
                else
                {
                        ewf_hdf_set("InitResult", "error");
                }

                /* gr909 */
                if(ptestResults.gr909_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("Gr909Result", "not_launched");
                }
                else if(ptestResults.gr909_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("Gr909Result", "success");
                }
                else if(ptestResults.gr909_status == VSS_TEST_FAILED)
                {
                        if(ptestResults.gr909_state == VSS_TEST_GR909_ERROR_VOLTAGE)
                        {
                                ewf_hdf_set("Gr909Error", "error_voltage");
                        }
                        else if(ptestResults.gr909_state == VSS_TEST_GR909_ERROR_RESIST)
                        {
                                ewf_hdf_set("Gr909Error", "error_resist");
                        }
                        else if(ptestResults.gr909_state == VSS_TEST_GR909_ERROR_UNHOOK)
                        {
                                ewf_hdf_set("Gr909Error", "error_offhook");
                        }
                        else if(ptestResults.gr909_state == VSS_TEST_GR909_ERROR_LINELOAD)
                        {
                                ewf_hdf_set("Gr909Error", "error_loadline");
                        }
                        else
                        {
                                ewf_hdf_set("Gr909Error", "unknown");
                        }

                        ewf_hdf_set("Gr909Result", "error");
                }
                else
                {
                        ewf_hdf_set("Gr909Result", "error");
                }

                /* ring */
                if(ptestResults.ring_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("RingResult", "not_launched");
                }
                else if(ptestResults.ring_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("RingResult", "success");
                }
                else
                {
                        ewf_hdf_set("RingResult", "error");
                }

                /* hookntone */
                if(ptestResults.hookntone_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("HookntoneResult", "not_launched");
                }
                else if(ptestResults.hookntone_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("HookntoneResult", "success");
                }
                else if(ptestResults.hookntone_status == VSS_TEST_FAILED_CHECK)
                {
                        ewf_hdf_set("HookntoneResult", "error");

                        if(ptestResults.hookntone_state == VSS_TEST_HOOKNTONE_ERROR_PICKUP)
                        {
                                ewf_hdf_set("HookntoneError", "pick_up");
                        }
                        else if(ptestResults.hookntone_state == VSS_TEST_HOOKNTONE_ERROR_HANGUP)
                        {
                                ewf_hdf_set("HookntoneError", "hang_up");
                        }
                        else if(ptestResults.hookntone_state == VSS_TEST_HOOKNTONE_ERROR_FT)
                        {
                                ewf_hdf_set("HookntoneError", "ft_standard");
                        }
                }
                else
                {
                        ewf_hdf_set("HookntoneResult", "error");
                }

                /* dtmf */
                if(ptestResults.dtmf_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("DtmfResult", "not_launched");
                }
                else if(ptestResults.dtmf_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("DtmfResult", "success");
                }
                else
                {
                        ewf_hdf_set("DtmfResult", "error");

                        nbu_string_printf(buffer, sizeof(buffer),
                                          "%s_%d",
                                          (ptestResults.dtmf_state - 1) % 2 == 0 ? "press" : "release",
                                          (ptestResults.dtmf_state - 1) / 2);
                        ewf_hdf_set("DtmfError", buffer);
                }
        }
        else if(nbu_string_matches(action, "reboot") == NBU_STRING_EQUAL)
        {
                ewf_utils_redirect(WEBAPP_URI_REBOOT);
        }
#ifndef NB5
        else if(nbu_string_matches(action, "dsltest") == NBU_STRING_EQUAL)
        {
                if(nbd_dsl_plugtest(&buf_xml, &buf_xml_size) == NBD_SUCCESS)
                {
                        xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                        if(xmln_root != NULL)
                        {
                                xmln_result = ezxml_child(xmln_root, "result");
                                if(xmln_result != NULL)
                                {
                                        ewf_hdf_set("dsltest_result",
                                                    ezxml_attr(xmln_result,
                                                               "value"));
                                }

                                ezxml_free(xmln_root);
                        }
                        else
                        {
                                nbu_log_error("Unable to parse xml string");
                        }

                        free(buf_xml);
                }
        }
#endif

        return;
}

AJAX_HANDLER
{

        const char *action = NULL, *step = NULL;
        char nextstep[16];
        vSS_testResults ptestResults;

#ifndef NB5
        char *buf_xml = NULL;
        size_t buf_xml_size;
        ezxml_t xmln_root, xmln_result;
        const char *val = NULL;
#endif
        ewf_hdf_cgi_parse();
        action = ewf_hdf_sget("action");
        if(action == NULL)
        {
                return;
        }

#ifndef NB5

        /* dsl_plugtest blockant 10 seconds */
        if(nbu_string_matches(action, "dsltest") == NBU_STRING_EQUAL)
        {
                if(nbd_dsl_plugtest(&buf_xml, &buf_xml_size) == NBD_SUCCESS)
                {
                        xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                        if(xmln_root != NULL)
                        {
                                xmln_result = ezxml_child(xmln_root, "result");
                                if(xmln_result != NULL)
                                {
                                        val = ezxml_attr(xmln_result, "value");

                                        ewf_hdf_set("dsltest_result", val);
                                }

                                ezxml_free(xmln_root);
                        }
                        else
                        {
                                nbu_log_error("Unable to parse xml string");
                        }

                        free(buf_xml);
                }
        }
#endif


        if(nbu_string_matches(action, "vSStest") == NBU_STRING_EQUAL)
        {
                step = ewf_hdf_sget("step");
                if(step == NULL)
                {
                        return;
                }
                ewf_hdf_set("action", action);
                ewf_hdf_set("vSStest_step", step);

                if(nbu_string_matches(step, "init") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_initScript();
                        (void) nbd_vSStest_init();
                        nbu_log_debug("init status = %d", ptestResults.init_status);
                }
                else if(nbu_string_matches(step, "gr909") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_gr909();
                        nbu_log_debug("ptestResults.gr909_status = %d", ptestResults.gr909_status);
                        nbu_log_debug("Gr909 status = %d", ptestResults.gr909_status);
                }
                else if(nbu_string_matches(step, "ring") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_ring();
                        nbu_log_debug("Ring status = %d", ptestResults.ring_status);
                }
                else if(nbu_string_matches(step, "hookntone") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_hookntone();
                        nbu_log_debug("Hookntone status = %d", ptestResults.hookntone_status);
                }
                else if(nbu_string_matches(step, "dtmf") == NBU_STRING_EQUAL)
                {
                        (void) nbd_vSStest_dtmf();
                        nbu_log_debug("Dtmf status = %d", ptestResults.dtmf_status);
                }
                else
                {
                        nbu_log_error("No step '%s' for vSStest", step);
                        return; /* RETURN */
                }
                /* Spy send event that we have new results on voice test */
                nbd_spy_event("voice-test");
        }



        /* analyze the result */
        if((nbu_string_matches(action, "vSStest_info") == NBU_STRING_EQUAL) || (nbu_string_matches(action, "vSStest") == NBU_STRING_EQUAL))
        {
                nbd_vSStest_getResult(&ptestResults);
                nbu_string_printf(nextstep, sizeof nextstep, "");
                /* step1: init */
                if(ptestResults.init_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("InitResult", "not_launched");
                }
                else if(ptestResults.init_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("InitResult", "success");
                        nbu_string_printf(nextstep, sizeof nextstep, "gr909");
                }
                else if(ptestResults.init_status == VSS_TEST_FAILED_CHECK)
                {
                        ewf_hdf_set("InitResult", "error");
                        ewf_hdf_set("InitError", "check_hook");
                        ewf_hdf_set("vSStest_step", "init");
                        nbu_string_printf(nextstep, sizeof nextstep, "init");
                }
                else if(ptestResults.init_status == VSS_TEST_IN_PROGRESS)
                {
                        ewf_hdf_set("InitResult", "in_progress");
                        nbu_string_printf(nextstep, sizeof nextstep, "");
                }
                else
                {
                        ewf_hdf_set("InitResult", "error");
                        ewf_hdf_set("vSStest_step", "init");
                        nbu_string_printf(nextstep, sizeof nextstep, "init");
                }

                /* step2: gr909 */
                if(ptestResults.gr909_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("Gr909Result", "not_launched");
                }
                else if(ptestResults.gr909_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("Gr909Result", "success");
                        nbu_string_printf(nextstep, sizeof nextstep, "ring");
                }
                else if(ptestResults.gr909_status == VSS_TEST_IN_PROGRESS)
                {
                        ewf_hdf_set("Gr909Result", "in_progress");
                        nbu_string_printf(nextstep, sizeof nextstep, "");
                }
                else if(ptestResults.gr909_status == VSS_TEST_FAILED)
                {
                        if(ptestResults.gr909_state == VSS_TEST_GR909_ERROR_VOLTAGE)
                        {
                                ewf_hdf_set("Gr909Error", "error_voltage");
                        }
                        else if(ptestResults.gr909_state == VSS_TEST_GR909_ERROR_RESIST)
                        {
                                ewf_hdf_set("Gr909Error", "error_resist");
                        }
                        else if(ptestResults.gr909_state == VSS_TEST_GR909_ERROR_UNHOOK)
                        {
                                ewf_hdf_set("Gr909Error", "error_offhook");
                        }
                        else if(ptestResults.gr909_state == VSS_TEST_GR909_ERROR_LINELOAD)
                        {
                                ewf_hdf_set("Gr909Error", "error_loadline");
                        }
                        else
                        {
                                ewf_hdf_set("Gr909Error", "unknown");
                        }
                        ewf_hdf_set("Gr909Result", "error");
                        nbu_string_printf(nextstep, sizeof nextstep, "gr909");
                        ewf_hdf_set("vSStest_step", "gr909");
                }
                else
                {
                        ewf_hdf_set("Gr909Result", "error");
                        ewf_hdf_set("vSStest_step", "gr909");
                        nbu_string_printf(nextstep, sizeof nextstep, "gr909");
                }

                /* step3: ring */
                if(ptestResults.ring_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("RingResult", "not_launched");
                }
                else if(ptestResults.ring_status == VSS_TEST_SUCCEEDED)
                {
                        nbu_string_printf(nextstep, sizeof nextstep, "hookntone");
                        ewf_hdf_set("RingResult", "success");
                }
                else if(ptestResults.ring_status == VSS_TEST_IN_PROGRESS)
                {
                        ewf_hdf_set("RingResult", "in_progress");
                        nbu_string_printf(nextstep, sizeof nextstep, "");
                }
                else
                {
                        ewf_hdf_set("RingResult", "error");
                        nbu_string_printf(nextstep, sizeof nextstep, "ring");
                        ewf_hdf_set("vSStest_step", "ring");
                }

                /* step4: hookntone */
                if(ptestResults.hookntone_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("HookntoneResult", "not_launched");
                }
                else if(ptestResults.hookntone_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("HookntoneResult", "success");
                        nbu_string_printf(nextstep, sizeof nextstep, "dtmf");
                }
                else if(ptestResults.hookntone_status == VSS_TEST_IN_PROGRESS)
                {
                        ewf_hdf_set("HookntoneResult", "in_progress");
                        nbu_string_printf(nextstep, sizeof nextstep, "");
                }
                else if(ptestResults.hookntone_status == VSS_TEST_FAILED_CHECK)
                {
                        ewf_hdf_set("vSStest_step", "hookntone");
                        ewf_hdf_set("HookntoneResult", "error");
                        nbu_string_printf(nextstep, sizeof nextstep, "hookntone");

                        if(ptestResults.hookntone_state == VSS_TEST_HOOKNTONE_ERROR_PICKUP)
                        {
                                ewf_hdf_set("HookntoneError", "pick_up");
                        }
                        else if(ptestResults.hookntone_state == VSS_TEST_HOOKNTONE_ERROR_HANGUP)
                        {
                                ewf_hdf_set("HookntoneError", "hang_up");
                        }
                        else if(ptestResults.hookntone_state == VSS_TEST_HOOKNTONE_ERROR_FT)
                        {
                                ewf_hdf_set("HookntoneError", "ft_standard");
                        }
                }
                else
                {
                        ewf_hdf_set("vSStest_step", "hookntone");
                        ewf_hdf_set("HookntoneResult", "error");
                        nbu_string_printf(nextstep, sizeof nextstep, "hookntone");
                }

                if(ptestResults.dtmf_status == VSS_TEST_NOT_RUN)
                {
                        ewf_hdf_set("DtmfResult", "not_launched");
                }
                else if(ptestResults.dtmf_status == VSS_TEST_IN_PROGRESS)
                {
                        ewf_hdf_set("DtmfResult", "in_progress");
                        nbu_string_printf(nextstep, sizeof nextstep, "");
                }
                else if(ptestResults.dtmf_status == VSS_TEST_SUCCEEDED)
                {
                        ewf_hdf_set("DtmfResult", "success");
                        nbu_string_printf(nextstep, sizeof nextstep, "");
                }
                else
                {
                        ewf_hdf_set("vSStest_step", "dtmf");
                        ewf_hdf_set("DtmfResult", "error");
                        nbu_string_printf(buffer, sizeof(buffer),
                                          "%s_%d",
                                          (ptestResults.dtmf_state - 1) % 2 == 0 ? "press" : "release",
                                          (ptestResults.dtmf_state - 1) / 2);
                        ewf_hdf_set("DtmfError", buffer);
                        nbu_string_printf(nextstep, sizeof nextstep, "dtmf");
                }

                ewf_hdf_set("vSStest_nextstep", nextstep);

        }
}
