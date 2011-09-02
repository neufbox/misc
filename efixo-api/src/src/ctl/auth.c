#include "api_common.h"
#include "ewf_session.h"
#include "ewf_crypto.h"

#include "nbu/string.h"

#define LOGIN_HASH_SIZE 64
#define PASS_HASH_SIZE 64

/* GET
 * new Token
 *
 * err_id:
 *
 *  205 = authentification disabled
 *
 */
API_REGISTER_METHOD(auth, getToken, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC)
{
        char buf[64];
        
	ewf_session_cleaner();
	
	if(ewf_session_count() > 64)
	{
		nbu_log_info("max session count reached !");
		api_set_custom_error("201", "Max token reached");

		ewf_session_cleaner();
		
		return;
	}

        if(nbd_nv_get("web_auth", buf, sizeof(buf)) == NBD_SUCCESS)
        {
                if(nbu_string_matches(buf, "off") == NBU_STRING_EQUAL)
                {
                        api_set_custom_error("205", "Authentification disabled");
                }
        }
        
	if (ewf_session_create(buffer, sizeof(buffer)) == EWF_SUCCESS)
	{
		api_set_value("auth.token", buffer);

                if(nbd_nv_get("web_auth", buffer, sizeof(buffer)) == NBD_SUCCESS)
                {
                        api_set_value("auth.method", buffer);
                }
	}
	else
	{
                nbu_log_error("Unable to create session !");
		api_set_error(API_ERROR_UNKNOWN);
	}
}

enum auth_method {
        auth_invalid = -1,
        auth_off = 0,
        auth_passwd,
        auth_button,
};

/* GET
 * try to check token
 *
 * err_id:
 *
 *  201 = Invalid token
 *  202 = Expired token
 *  203 = Button wasn't pushed !
 *  204 = Invalid login and/or password !
 *  205 = Authentification disabled
 *
 */
API_REGISTER_METHOD(auth, checkToken, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PUBLIC)
{
	char *token = NULL, *hash = NULL;
	ewf_session_t *session = NULL;
	int presession_timeout = 0;
        struct timeval tv;
        enum auth_method auth_method = auth_invalid;
        
        char web_auth[128];
        
        char login[NBD_BUFFER_SIZE];
	char password[NBD_BUFFER_SIZE];
        
	char* login_hashed = NULL, *login_hash = NULL;
	char* password_hashed = NULL;
        
	char arg_login[LOGIN_HASH_SIZE + 1];
	char arg_password[PASS_HASH_SIZE + 1];
	
	ewf_session_cleaner();
	
	if (ewf_fastcgi_get_parameter("token", &token) == EWF_SUCCESS)
	{
		if(ewf_session_get(token, &session) == EWF_SUCCESS)
		{
                        /* Authentification */
                        if(nbd_nv_get("web_auth", web_auth, sizeof(web_auth)) != NBD_SUCCESS)
                        {
                                nbu_log_error("Unable to get nbd web_auth value");
                                api_set_error(API_ERROR_UNKNOWN);
                                return;
                        }
                        
                        if(nbu_string_matches(web_auth, "passwd") == NBU_STRING_EQUAL)
                        {
                                presession_timeout = PRESESSION_TIMEOUT_FAST;
                        }
                        else
                        {
                                presession_timeout = PRESESSION_TIMEOUT;
                        }

			/* have session, sanity check ! */
			if(session->activated == 1)
			{
				/* the session is already activated ! */
				/* Send error message as if token doesn't exist */
				api_set_custom_error("201", "Invalid session");
				return;
			}

			gettimeofday(&tv, NULL);
			if(session->tv.tv_sec + presession_timeout < tv.tv_sec  )
			{
				/* timeout */
				ewf_session_remove(session->id);
				api_set_custom_error("202", "Session for validation timeout");
				return;
			}

			/* the session seems to be correct continue ... */

                        if(nbu_string_matches(web_auth, "all") == NBU_STRING_EQUAL)
                        {
                                if (ewf_fastcgi_get_parameter("hash", &hash) == EWF_SUCCESS)
                                {
                                        /* login/password auth */
                                        auth_method = auth_passwd;
                                }
                                else
                                {
                                        /* auth button ? */
                                        auth_method = auth_button;
                                }
                        }
                        else if(nbu_string_matches(web_auth, "button") == NBU_STRING_EQUAL)
                        {
                                auth_method = auth_button;
                        }
                        else if(nbu_string_matches(web_auth, "passwd") == NBU_STRING_EQUAL)
                        {
                                if (ewf_fastcgi_get_parameter("hash", &hash) == EWF_SUCCESS)
                                {
                                        /* login/password auth */
                                        auth_method = auth_passwd;
                                }
                                else
                                {
                                        api_set_error(API_ERROR_NEED_ARG);
                                        return;
                                }
                        }
                        else
                        {
				api_set_custom_error("205", "Authentification disabled");
                                return;
                        }
                        
                        if(auth_method == auth_button)
                        {
                                /* test if user has pushed service button */
                                if(nbd_status_get("web_autologin", 
                                                  buffer, 
                                                  sizeof(buffer)) != NBD_SUCCESS)
                                {
                                        /* unable to get web_autologin */
                                        nbu_log_error("Unable to get nbd web_autologin value");
                                        api_set_error(API_ERROR_UNKNOWN);
                                        return;
                                }

                                if(nbu_string_matches(buffer, 
                                                      "on") == NBU_STRING_EQUAL)
                                {
                                        /* validate session ! */
                                        ewf_session_validate(session);
                                        
                                        /* set push button to off */
                                        nbd_halt_webautologin();
                                        
                                        /* the user has pushed service button ! */
                                        api_set_value("auth.token", token);
                                }
                                else
                                {
                                        /* button no pushed */
                                        api_set_custom_error("203", "Push button not pushed");
                                        return;
                                }
                        }
                        else if(auth_method == auth_passwd)
                        {
                                if(strlen(hash) != LOGIN_HASH_SIZE + PASS_HASH_SIZE)
                                {
                                        api_set_custom_error("204", "Invalid login and/or password");
                                        return;
                                }

                                nbu_string_copy(arg_login, sizeof(arg_login), hash);
                                arg_login[sizeof(arg_login) - 1] = '\0';
                                
                                nbu_string_copy(arg_password, sizeof(arg_password), hash + LOGIN_HASH_SIZE);
                                arg_password[sizeof(arg_password) - 1] = '\0';

                                /* check login */
                                if(nbd_nv_get("web_login", login, sizeof login) != NBD_SUCCESS)
                                {
                                        nbu_log_error("Unable to get nbd web_login value");
                                        api_set_error(API_ERROR_UNKNOWN);
                                        return;
                                }
                                
                                if(ewf_crypto_sha256_hash(login, &login_hash) != EWF_SUCCESS)
                                {
                                        nbu_log_error("login sha256 hash failed");
                                        api_set_error(API_ERROR_UNKNOWN);
                                        return;
                                }

                                if(ewf_crypto_hmac_sha256_mac(token, login_hash, &login_hashed) != EWF_SUCCESS)
                                {
                                        free(login_hash);
                                        nbu_log_error("login hmac sha256 hash failed");
                                        api_set_error(API_ERROR_UNKNOWN);
                                        return;
                                }
	
                                nbu_log_debug("login hashed = %s", login_hashed);
                                
                                if (nbu_string_compare(arg_login, login_hashed) != NBU_SUCCESS)
                                {
                                        /* login not match */
                                        free(login_hash);
                                        free(login_hashed);
		
                                        api_set_custom_error("204", "Invalid login and/or password");
                                        return;
                                }
                                free(login_hash);
                                free(login_hashed);
                                login_hash = NULL;
                                login_hashed = NULL;
                                
                                /* check password */
                                if(nbd_nv_get("web_password", password, sizeof password) != NBD_SUCCESS)
                                {
                                        nbu_log_error("Unable to get nbd web_password value");
                                        api_set_error(API_ERROR_UNKNOWN);
                                        return;
                                }

                                if(ewf_crypto_hmac_sha256_mac(token, password, &password_hashed) != EWF_SUCCESS)
                                {
                                        nbu_log_error("password hmac sha256 hash failed");
                                        api_set_error(API_ERROR_UNKNOWN);
                                        return;
                                }

                                nbu_log_debug("password hashed = %s", password_hashed);
                                
                                if (nbu_string_compare(arg_password, password_hashed) != NBU_STRING_EQUAL)
                                {
                                        /* password not match */
                                        free(password_hashed);
		
                                        api_set_custom_error("204", "Invalid login and/or password");
                                        return;
                                }
                                free(password_hashed);

                                /* !! all is ok !! */
                                
                                /* validate session ! */
                                ewf_session_validate(session);
                                api_set_value("auth.token", token);
                        }
		}
		else
		{
			/* this token doesn't exist in session list */
			api_set_custom_error("201", "Invalid session");
		}
	}
	else
	{
		api_set_error(API_ERROR_NEED_ARG);
	}
}
