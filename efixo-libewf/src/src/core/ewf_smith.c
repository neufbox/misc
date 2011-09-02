#include "ewf/ewf.h"
#include "ewf/ewf_crypto.h"
#include "ewf/ewf_utils.h"
#include "ewf/ewf_hdf.h"

#include "ewf/core/ewf_core.h"
#include "ewf/core/ewf_smith.h"
#include "ewf/core/ewf_session.h"
#include "ewf/core/ewf_config.h"
#include "ewf/core/ewf_matrix.h"

#include <sys/time.h>
#include <time.h>

/**************************************************************/
/*                STATIC FUNCTION PROTOTYPES                  */
/**************************************************************/

static int ewf_smith_check_session(void);
static int ewf_smith_hmac(const char *value,
			  const char *hmac_key,
			  char *buf, size_t buf_size);
static int ewf_smith_full_hash(const char *value,
			       const char *hmac_key,
			       char *buf, size_t buf_size);
static int ewf_smith_login_passwd(ewf_session_t *zombie_session);

/**************************************************************/
/*                PUBLIC FUNCTION DEFINITIONS                 */
/**************************************************************/

/*
 * int ewf_smith_access_control(const ewf_page_t *ewf_page)
 *
 * 0) authentification is ON ?
 *  -> yes                         =>  go (1)
 *  -> no                          =>  return EWF_SMITH_CTL_AUTH_OFF
 * 
 * 1) check privacy of page
 *  -> if PUBLIC and
 *       -> user NOT LOGGED        =>  return EWF_SMITH_CTL_PUBLICPAGE
 *       -> user LOGGED            =>  return EWF_SMITH_CTL_SUCCESS
 *  -> if PRIVATE                  =>  go (2)
 *
 * 2) get Cookie.sid
 *  -> if GOOD                     =>  return EWF_SMITH_CTL_SUCCESS
 *  -> otherwise                   =>  return EWF_SMITH_CTL_NOT_ALLOWED
 *
 */
int ewf_smith_access_control(const ewf_page_t *page)
{
	int ret = EWF_SMITH_CTL_NOT_ALLOWED;
	char auth[16];
	
	ewf_config_get("web_auth", auth, sizeof(auth));
	
	if(nbu_string_matches(auth, "off") == NBU_STRING_EQUAL)
	{
		/* No ident/auth protection */
		ret = EWF_SMITH_CTL_AUTH_OFF;
	}
	else
	{
		if(ewf_smith_check_session() == EWF_SUCCESS)
		{
			ret = EWF_SMITH_CTL_SUCCESS;
		}
		else
		{
			if(page->access_policy == EWF_ACCESS_PUBLIC)
			{
				ret = EWF_SMITH_CTL_PUBLICPAGE;
			}
			else
			{
				ret = EWF_SMITH_CTL_NOT_ALLOWED;
			}
		}
	}

	return ret;
}

/*
 * int ewf_smith_login(void)
 *
 * A) trying to retrieve a zombie (ie. "no actived") session
 *   -> try to get Query.zsid value
 *     -> if NULL                  =>  go to (0)
 *     -> otherwise, try to get
 *        the zombie session
 *       -> if NULL                =>  return EWF_SMITH_LOGIN_INVALID_SESSION
 *       -> otherwise              =>  save zombie session ptr and go to (0)
 *
 * 0) get Query.method (if not, use default login mechanism, ie. passwd)
 *   -> if method = 'passwd'       =>  go to (1)
 *   -> if method unknown          =>  go to (2)
 *
 * 1) get Query.login and Query.password and validate it
 *   -> if GOOD                    =>  return EWF_SMITH_LOGIN_SUCCESS
 *   -> otherwise                  =>  return EWF_SMITH_LOGIN_INVALID_ARGS
 *
 * 2) get ewf_matrix.login_alt_method function
 *   -> if NULL                    =>  return EWF_SMITH_LOGIN_METHOD_NOT_ALLOWED
 *   -> otherwise, call it         =>  return the login_alt_method func answer
 *
 * more:
 * -----
 * 
 *  -> if SUCCESS, create cookie named 'sid' with the 'session id created' 
 *     as value
 *
 *  -> if method failed            =>  return EWF_SMITH_LOGIN_FAILED
 *
 *  -> if an internal error occur  =>  return EWF_SMITH_LOGIN_ERROR
 *
 * explanations:
 * -------------
 *
 *  -> The (A) point permit to have a "simple challenge mecanism":
 *    I) create a zombie session and return the session id to the user
 *    II) the user use the zombie session id to compute something
 *        and give this with cleartext 'zsid' value to ewf_smith_login
 *    III) ewf_smith_login check if zombie session exist and the 
 *        "login method" check the compute value is good
 *    IV) if success, the zombie session become a good session
 */
int ewf_smith_login(void)
{
	const char *method = NULL, *zsid = NULL;
	int ret = EWF_SMITH_LOGIN_METHOD_NOT_ALLOWED;
	ewf_session_t *zombie_session = NULL;
	short int zombie_session_local_init = 0;
	struct timeval tv;
	char auth[16];
	
	/* 
	 * try to retrieve the zombie session 
	 * or create one LOCAL zombie session
	 */
	zsid = hdf_get_valuef(cgi->hdf, "Query.zsid");
        
	if(zsid != NULL
           && strlen(zsid) > 0)
	{
                nbu_log_debug("zsid = '%s' (retrieve from user)", zsid);
                
		zombie_session = ewf_session_list_get(zsid);
		if(zombie_session == NULL)
		{
			nbu_log_error("Invalid session !");
			ret = EWF_SMITH_LOGIN_INVALID_SESSION;
                        goto error_login;
		}
	}
	else
	{
                nbu_log_debug("zsid unavailable, create one ...");
                
		zombie_session = ewf_session_create();
		if(zombie_session == NULL)
		{
			nbu_log_error("Unable to allocated new session !");
			ret = EWF_SMITH_LOGIN_ERROR;
                        goto error_login;
		}
                
                zsid = zombie_session->id;
		zombie_session_local_init = 1;
	}
	
	/* get the wanted auth method */
	method = hdf_get_valuef(cgi->hdf, "Query.method");
	if(method == NULL)
	{
		method = "passwd";
	}
	
	/* verify the wanted auth method is authorized */
	ewf_config_get("web_auth", auth, sizeof(auth));
	
	if(nbu_string_matches(auth, "all") != NBU_STRING_EQUAL
	   && nbu_string_compare(method, 
				 auth) != NBU_STRING_EQUAL)
	{
                ret = EWF_SMITH_LOGIN_METHOD_NOT_ALLOWED;
                goto error_login;
	}
	
	/* call the good auth method */
	if(nbu_string_matches(method, "passwd") == NBU_STRING_EQUAL)
	{
		ret = ewf_smith_login_passwd(zombie_session);
	}
	else
	{
		ret = (ewf_matrix_ops.login_alt_method != NULL
		       ? ewf_matrix_ops.login_alt_method(zombie_session, method)
		       : EWF_SMITH_LOGIN_METHOD_NOT_ALLOWED);
	}

	/* if authentification success */
	if(ret == EWF_SMITH_LOGIN_SUCCESS)
	{
		nbu_log_debug("The auth method success, try to make feel "
			      "good the session !");
		
		/* make feel good the session */
                if(zombie_session_local_init)
                {
                        if(ewf_session_list_add(zombie_session) != EWF_SUCCESS)
                        {
                                nbu_log_error("Error when adding local "
                                              "session");
                                ret = EWF_SMITH_LOGIN_ERROR;
                                goto error_login;
                        }
                }

		gettimeofday(&tv, NULL);
		
		zombie_session->activated = 1;
		zombie_session->tv = tv;

		/* auth key is the challenge number */
		cgi_cookie_set(cgi, "sid", zombie_session->id, 
			       "/", NULL, "", 0, 0);

                return EWF_SMITH_LOGIN_SUCCESS;
	}
	else
	{
		/* cleanup */
		if(!zombie_session_local_init)
                {
                        ewf_session_list_remove(zsid);
                }
	}
        
error_login:
        if(zombie_session_local_init)
        {
                free(zombie_session);
        }
        
	return ret;
}

/*
 * int ewf_smith_logout(void)
 *
 * -> if success                   =>  return EWF_SMITH_LOGOUT_SUCCESS
 * -> otherwise                    =>  return EWF_SMITH_LOGOUT_FAILED
 *
 */
int ewf_smith_logout(void)
{
	char *session_id = NULL;
	int ret = EWF_ERROR;

	/* Get cookie auth (session id) */
	session_id = hdf_get_valuef(cgi->hdf, "Cookie.sid");
	if(session_id != NULL
	   && ewf_session_list_remove(session_id) == EWF_SUCCESS) 
	{
		/* remove cookie */
		cgi_cookie_clear(cgi, "sid", NULL, "/");

		ret = EWF_SUCCESS;
	}
	
	return ret;
}

/**************************************************************/
/*                STATIC FUNCTION DEFINITIONS                 */
/**************************************************************/

static int ewf_smith_check_session(void)
{
	char *session_id = NULL;

	ewf_session_t* ewf_session = NULL;
	struct timeval tv;
  
	int ret = EWF_ERROR;
	
	/* Get cookie auth (session id) */
	session_id = hdf_get_valuef(cgi->hdf, "Cookie.sid");
	if(session_id != NULL) 
	{
		nbu_log_debug("Cookie 'sid' found");

		/* retrieve session */
		ewf_session = ewf_session_list_get(session_id);
		
		if(ewf_session != NULL)
		{
			nbu_log_debug("The session has been found");
			
			gettimeofday(&tv, NULL);
			if (tv.tv_sec > (ewf_session->tv.tv_sec 
					 + SESSION_TIMEOUT)) 
			{
				/* session expired */
				nbu_log_debug("The session has expired");
				
				ewf_session_list_remove(session_id);
			} 
			else 
			{
				/* session valid - update session time */
				ewf_session->tv = tv;
				
				ret = EWF_SUCCESS;
			}
		}
	}
	
	return ret;
}

static int ewf_smith_hmac(const char *value,
			  const char *hmac_key,
			  char *buf, size_t buf_size)
{
	char *hash_mac = NULL;

	if(ewf_crypto_hmac_sha256_mac(hmac_key, value, &hash_mac) 
	   != EWF_SUCCESS)
	{
		nbu_log_error("Unable to hmac '%s'", value);
		
		return EWF_ERROR;
	}
	
	nbu_string_copy(buf, buf_size, hash_mac);
	
	free(hash_mac);
	
	return EWF_SUCCESS;
}

static int ewf_smith_full_hash(const char *value,
			       const char *hmac_key,
			       char *buf, size_t buf_size)
{
	char *hash = NULL;
	int ret = EWF_ERROR;
		
	if(ewf_crypto_sha256_hash(value, &hash) != EWF_SUCCESS)
	{
		nbu_log_error("Unable to sha256 hash '%s'", value);
		
		return EWF_ERROR;
	}

	ret = ewf_smith_hmac(hash, hmac_key, buf, buf_size);

	free(hash);
	
	return EWF_SUCCESS;
}
			  
/*
 * static int ewf_smith_login_passwd(ewf_session_t *zombie_session)
 * 
 * login and password authentification method
 *
 * -> if user has use challenge method, the hash looks like this:
 *
 *   -----------------------------------------------------
 *   | LOGIN HASHED (64 car.) |  PASSWD HASHED (64 car.) |
 *   -----------------------------------------------------
 *
 *
 */
static int ewf_smith_login_passwd(ewf_session_t *zombie_session)
{
	char buf[256];
	char login[65], password[65], hash[129];
	unsigned int len_hash = 0;
	char cfg_login[65], cfg_password[65];
	
	/*
	 * try to get the hash form value and check his len
	 */
	if(ewf_hdf_sget_copy("hash",
			     hash,
			     sizeof hash) == EWF_SUCCESS)
	{
		len_hash = strlen(hash);
	}

	if(len_hash != 0 && len_hash != (LOGIN_HASH_LEN + PASSW_HASH_LEN))
	{
		nbu_log_error("Invalid hash size");
		
		return EWF_SMITH_LOGIN_INVALID_ARGS;
	}

	if(len_hash == 0)
	{
		/* 
		 * get cleartext login and password value, hash them
		 */
	
		/* login */
		if(ewf_hdf_sget_copy("login", buf, sizeof buf)
		   != EWF_SUCCESS)
		{
			nbu_log_error("Unable to get login value");
		
			return EWF_SMITH_LOGIN_INVALID_ARGS;
		}
		
		if(ewf_smith_full_hash(buf, zombie_session->id, 
				       login, 
				       sizeof login) != EWF_SUCCESS)
		{
			return EWF_SMITH_LOGIN_ERROR;
		}
		
		/* password */
		if(ewf_hdf_sget_copy("password", buf, sizeof buf)
		   != EWF_SUCCESS)
		{
			nbu_log_error("Unable to get password value");
		
			return EWF_SMITH_LOGIN_INVALID_ARGS;
		}
		
		if(ewf_smith_full_hash(buf, zombie_session->id, 
				       password, 
				       sizeof password) != EWF_SUCCESS)
		{
			return EWF_SMITH_LOGIN_ERROR;
		}
		
	}
	else
	{
		/*
		 * user has used the challenge method, good guy !
		 * decompose the big hash
		 */
		nbu_string_copy(login, sizeof(login), hash);
		nbu_string_copy(password, sizeof(password), 
				hash + LOGIN_HASH_LEN);
	}

	/*
	 * So the good moment, compare things give by user and the correct
	 * login / password.
	 *
	 *  login stored is cleartext. password stored is already hashed.
	 */
	
	/* login */
	ewf_config_get("web_login", cfg_login, sizeof(cfg_login));
	
	if(ewf_smith_full_hash(cfg_login, zombie_session->id, 
			       buf, 
			       sizeof buf) != EWF_SUCCESS)
	{
		return EWF_SMITH_LOGIN_ERROR;
	}
	
	if (nbu_string_compare(login, buf) != NBU_SUCCESS)
	{
		/* login not match */
		return EWF_SMITH_LOGIN_INVALID_ARGS;
	}

	/* password */
	ewf_config_get("web_password", cfg_password, sizeof(cfg_password));
	
	if(ewf_smith_hmac(cfg_password, zombie_session->id, 
			  buf, 
			  sizeof buf) != EWF_SUCCESS)
	{
		return EWF_SMITH_LOGIN_ERROR;
	}
	
	if (nbu_string_compare(password, buf) != NBU_SUCCESS)
	{
		/* password not match */
		return EWF_SMITH_LOGIN_INVALID_ARGS;
	}
	
	return EWF_SMITH_LOGIN_SUCCESS;
}
