#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>
#include <ewf/core/ewf_fcgi.h>
#include <ewf/core/ewf_smith.h>
#include <ewf/core/ewf_session.h>

#include "webapp.h"

#include "nbl.h"
#include "nbd.h"
#include "dbl.h"

PAGE_DFA("/login", EWF_ACCESS_PUBLIC);

DISPLAY_HANDLER
{
        const char *page_ref = NULL;

	if((page_ref = ewf_hdf_sget("page_ref")) != NULL)
	{
                ewf_hdf_set_env("Page.Referer", page_ref);
	}

	nv_to_hdf("web_auth", "method");
#ifndef CIBOX
	status_to_hdf("web_autologin", "web_auth_button_status");
#endif

	return;
}

FORM_HANDLER
{
	int ret = EWF_ERROR;

        const char *page_ref = NULL;
	char page_ref_norm[64];

        /* Get page_ref */
        if((page_ref = ewf_hdf_sget("page_ref")) != NULL
           && strcmp(page_ref, URI_PAGE_LOGIN) != 0)
        {
                nbu_string_copy(page_ref_norm, sizeof page_ref_norm,
                                page_ref);
        }
        else
        {
                nbu_string_copy(page_ref_norm, sizeof page_ref_norm,
                                URI_PAGE_LOGIN);
        }

        ret = ewf_smith_login();
        if(ret == EWF_SMITH_LOGIN_SUCCESS)
        {
#ifndef CIBOX
                /* halt web autologin button method */
                nbd_halt_webautologin();
#endif

                /* identification success, redirect to page_ref */
                ewf_utils_redirect(page_ref_norm);
        }
        else
        {
                ewf_hdf_set_env("Page.Referer", page_ref_norm);

                switch(ret)
                {
                case EWF_SMITH_LOGIN_INVALID_ARGS:
                        ewf_hdf_set("Error.login", "LOGINORPASS_INVALID");
                        break;
                case EWF_SMITH_LOGIN_INVALID_SESSION:
                        ewf_hdf_set("Error.login", "SESSION_INVALID");
                        break;
                case EWF_SMITH_LOGIN_METHOD_NOT_ALLOWED:
                        ewf_hdf_set("Error.login", "METHOD_NOT_ALLOWED");
                        break;
                case WEBAPP_SMITH_AUTOLOGIN_OFF:
                        ewf_hdf_set("Error.login", "AUTOLOGIN_OFF");
                        break;
                case EWF_SMITH_LOGIN_ERROR:
                default:
                        ewf_hdf_set("Error.login", "SYSTEM_ERROR");
                        break;
                }
        }

	return;
}

AJAX_HANDLER
{
        ewf_session_t *session = NULL;
        const char *action = NULL;

        ewf_hdf_cgi_parse();

        action = ewf_hdf_sget("action");

        if(action != NULL
           && nbu_string_matches(action, "challenge") == NBU_STRING_EQUAL)
        {
                ewf_hdf_set("action", action);

                /* create a zombie session with a challenge id */
                session = ewf_session_create();
                if(session != NULL)
                {
                        if(ewf_session_list_add(session) == EWF_SUCCESS)
                        {
                                ewf_hdf_set("challenge", session->id);
                        }
                        else
                        {
                                free(session);
                        }
                }
        }
#ifndef CIBOX
        else
        {
                if(nbd_status_get("web_autologin",
                                  buffer,
                                  sizeof buffer) == NBD_SUCCESS)
                {
                        ewf_hdf_set("autologin", buffer);
                }
        }
#endif

	return;
}
