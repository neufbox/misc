#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_crypto.h>
#include <ewf/ewf_utils.h>
#include <ewf/core/ewf_smith.h>

#include "nbl.h"
#include "nbd.h"
#include "dbl.h"
#include "webapp.h"

PAGE_DF("/maintenance/admin", EWF_ACCESS_PRIVATE);

/* (AntiCSRF) get the good method to verify identity when trying to change
 *            the login, password or authentification scheme
 *
 *      CURRENT AUTH SYSTEM           |      CHECK
 * -----------------------------------|------------------------------------
 * password default (wpa key)         |      passwd check
 * all/passwd method, passwd changed  |      passwd check
 * button method                      |      button check (CIBOX -> no check)
 * desactivated                       |      button check (CIBOX -> no check)
 */
static int get_validation_method(char *buf, size_t buf_size)
{
        char web_auth[16];
        int ret = -1;

        if(nbd_user_config_get("web_auth",
                      web_auth,
                      sizeof(web_auth)) != NBD_SUCCESS)
        {
                nbu_log_error("Get nvram 'web_auth' failed");
                return ret;
        }

        ret = 0;

        if(nbu_string_matches(web_auth, "off") == NBU_STRING_EQUAL
           || nbu_string_matches(web_auth, "button") == NBU_STRING_EQUAL)
        {
                /* current web_auth is off or button method */
#ifdef CIBOX
                nbu_string_copy(buf, buf_size, "off");
#else
                nbu_string_copy(buf, buf_size, "button");
#endif
        }
        else
        {
                nbu_string_copy(buf, buf_size, "passwd");
        }

        return ret;
}

DISPLAY_HANDLER
{
        char checkmethod[16];

        nv_to_hdf("web_login", "web_login");
        nv_to_hdf("web_auth", "web_auth");

        if(get_validation_method(checkmethod, sizeof(checkmethod)) == 0)
        {
                ewf_hdf_set("WebAuthChangeCheckMethod", checkmethod);
        }
}

FORM_HANDLER
{
        const char *web_auth = NULL,
                *web_password = NULL,
                *web_password_check = NULL,
                *web_login = NULL,
                *web_old_login = NULL,
                *web_old_password = NULL;
        char web_curr_login[128],
                web_curr_password[128],
                web_autologin[8];
        char checkmethod[16],
                web_password_hash[65];
        struct trx trx;
        int ret = 0;

        char *passwd_hash = NULL;

        EWF_HDF_GET(web_auth);

        /* auth check before set - anti CRSF - */
        if(get_validation_method(checkmethod, sizeof(checkmethod)) != 0)
        {
                ewf_hdf_form_set_error("system");
                return;
        }

        if(nbu_string_matches(checkmethod, "passwd") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(web_old_login);
                EWF_HDF_GET(web_old_password);

                if(nbd_user_config_get("web_login",
                              web_curr_login,
                              sizeof(web_curr_login)) != NBD_SUCCESS)
                {
                        nbu_log_error("Unable to get nv web_login !");
                        return;
                }

                if(nbd_user_config_get("web_password",
                              web_curr_password,
                              sizeof(web_curr_password)) != NBD_SUCCESS)
                {
                        nbu_log_error("Unable to get nv web_password !");
                        return;
                }

                /* check login */
                if(nbu_string_compare(web_curr_login,
                                      web_old_login) != NBU_STRING_EQUAL)
                {
                        ewf_hdf_set_error("web_old_login", "no_match");
                        return;
                }

                /* check password */
                ret = ewf_crypto_sha256_hash(web_old_password, &passwd_hash);
                if(ret != EWF_SUCCESS)
                {
                        nbu_log_error("sha256(%s) failed",
                                      web_old_password);
                        ewf_hdf_form_set_error("unknown");
                        return;
                }

                ret = nbu_string_compare(web_curr_password, passwd_hash);
                free(passwd_hash);
                if(ret != NBU_STRING_EQUAL)
                {
                        ewf_hdf_set_error("web_old_password", "no_match");
                        return;
                }
        }
        else if(nbu_string_matches(checkmethod, "button") == NBU_STRING_EQUAL)
        {
                STATUS_GET(web_autologin);

                if(nbu_string_matches(web_autologin, "on") != NBU_STRING_EQUAL)
                {
                        ewf_hdf_form_set_error("autologin_off");
                        return;
                }
        }

        nbu_log_debug("Auth check pass");

        /* other checks */
        if(nbu_string_matches(web_auth, "all") == NBU_STRING_EQUAL
           || nbu_string_matches(web_auth, "passwd") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(web_login);
                EWF_HDF_GET(web_password);
                EWF_HDF_GET(web_password_check);

                /* want new password ? */
                if(strlen(web_password) > 0)
                {
                        ret = nbu_string_compare(web_password,
                                                 web_password_check);
                        if(ret != NBU_STRING_EQUAL)
                        {
                                ewf_hdf_set_error("web_password",
                                                  "no_match");
                                return;
                        }

                        ret = ewf_crypto_sha256_hash(web_password,
                                                     &passwd_hash);
                        if(ret != EWF_SUCCESS)
                        {
                                ewf_hdf_form_set_error("unknown");
                                return;
                        }

                        nbu_string_copy(web_password_hash,
                                        sizeof(web_password_hash),
                                        passwd_hash);

                        free(passwd_hash);
                }
        }
        else if(nbu_string_matches(web_auth, "off") == NBU_STRING_EQUAL)
        {
                /* remove user session */
                ewf_smith_logout();
        }

        /* set */
        NV_TRX_INIT(trx);

        NV_SET_WTRX(trx, "web_auth", web_auth);

        if(nbu_string_matches(web_auth, "all") == NBU_STRING_EQUAL
           || nbu_string_matches(web_auth, "passwd") == NBU_STRING_EQUAL)
        {
                NV_SET_WTRX(trx, "web_login", web_login);

                if(strlen(web_password) > 0)
                {
                        NV_SET_WTRX(trx, "web_password", web_password_hash);
                }
        }

        ret = NV_TRX_XCOMMIT(trx);
        if(ret == NV_COMMIT_SUCCESS)
        {
                ewf_hdf_form_set_success("done");

                /* reset button check */
                if(nbu_string_matches(checkmethod,
                                      "button") == NBU_STRING_EQUAL)
                {
                        /* halt web autologin button method */
                        nbd_halt_webautologin();
                }

                if(nbu_string_matches(web_auth, "off") == NBU_STRING_EQUAL)
                {
                        /* reload page */
                        ewf_utils_redirect(WEBAPP_URI_WEBADMIN);
                }
        }

        return;
}
