#include "ClearSilver.h"
#include "api_common.h"
#include "ewf.h"
#include "ewf_session.h"

#include "nbu/nbu.h"
#include "nbu/string.h"

#ifdef DMALLOC
#define FINI_DMALLOC 1
#  include "dmalloc.h"
#endif

struct api_error_entry error_table[] = {
	{"0", "Unknown error"}, /* API_ERROR_UNKNOWN */
	{"111", "Format not found"}, /* API_ERROR_FORMAT_NOT_FOUND */
	{"112", "Method not found"}, /* API_ERROR_METHOD_NOT_FOUND */
	{"113", "Need argument(s)"}, /* API_ERROR_NEED_ARG */
	{"114", "Invalid argument(s)"}, /* API_ERROR_INVALID_ARG */
	{"115", "Need authentication"}, /* API_ERROR_NEED_AUTH */
	{"120", "The box being upgrade"}, /* API_ERROR_BOX_UPGRADING */
};

void api_init( void )
{
    api_signal_setup(  );
    nbu_log_open( "fastcgi" );

    nbu_log_info( "api starting" );

#ifdef DMALLOC
    dmalloc_debug_setup( "log-stats,log-non-free,check-fence,log=/tmp/apid.%p" );
#endif

    nbd_connect(  );

    nbu_log_info( "api started" );
}

void api_register_method( const char *method_name, int request_method,
			  int access_policy, void ( *action_handler ) ( void ) )
{
    api_method_t *m = NULL;

    if ( ( methods.initialized == 0 )
	 && ( ewf_hashtable_create( &methods.list, 97 ) == EWF_ERROR ) ) {
	nbu_log_error( "hash table creation failed" );
	return;
    }

    methods.initialized = 1;

    m = calloc( 1, sizeof( api_method_t ) );
    if ( m == NULL ) {
	nbu_log_error( "allocation failed for method '%s'", method_name );
	return;
    }

    m->request_method = request_method;
    m->access_policy = access_policy;
    m->action_handler = action_handler;

    if ( ewf_hashtable_insert( &methods.list, method_name, ( void * ) &m )
	 == EWF_ERROR ) {
	nbu_log_error( "hash table is full, method '%s' not registered", method_name );
	return;
    }

    nbu_log_debug( "method '%s' registered", method_name );

    return;
}

void api_cleanup( void )
{
    /* free the memory used by the FastCGI library */
    OS_LibShutdown(  );

    /* free the pointers to the methods hash table */
    if ( methods.initialized == 1 ) {
	nbu_log_debug( "freeing methods hash table" );
	ewf_hashtable_destroy( &methods.list );
    }

    /* disconnect from nbd */
    nbd_disconnect(  );

#ifdef DMALLOC
    dmalloc_shutdown(  );
#endif

    nbu_log_info( "api stopped" );
    nbu_log_close(  );
}

void api_begin( void )
{
    NEOERR *err;

    err = cgi_init( &cgi, NULL );
    nerr_ignore( &err );
}

void api_end( void )
{
    cgi_destroy( &cgi );
}

void api_dispatch( void )
{
    api_method_t *m = NULL;
    char *uri = NULL;
    char *name = NULL;

    api_set_value( "version", "1.0" );
    api_set_value( "env.uname", BOXNAME);

    ewf_fastcgi_set_content_type( "text/xml" );

    uri = ewf_http_request_uri(  );
    if ( uri == NULL ) {
	nbu_log_info( "uri is NULL" );
	api_set_error( API_ERROR_METHOD_NOT_FOUND );
	ewf_fastcgi_display( "api/generic.xml" );
	return;
    }

/*        nbu_log_debug("uri: '%s'", uri); */
/*        nbu_log_debug("Query.Method: '%s'", hdf_get_valuef(cgi->hdf, "Query.method")); */
/*        nbu_log_debug("CGI.QueryString: '%s'", hdf_get_valuef(cgi->hdf, "CGI.QueryString")); */
       
    if ( ewf_fastcgi_get_parameter( "method", &name ) == EWF_ERROR ) {
	nbu_log_info( "method param is NULL" );
	api_set_error( API_ERROR_METHOD_NOT_FOUND );
	ewf_fastcgi_display( "api/generic.xml" );
	return;
    }

    if ( ewf_hashtable_find( &methods.list, name, ( void * ) &m ) == EWF_ERROR ) {
	nbu_log_info( "method '%s' not found", name );
	api_set_error( API_ERROR_METHOD_NOT_FOUND );
	ewf_fastcgi_display( "api/generic.xml" );
	return;
    }

    nbu_log_info( "method '%s' found", name );
    api_handle_request( m, name );

    return;
}

void api_handle_request( api_method_t * m, const char *name )
{
    ewf_http_request_method_t reqm;
    ewf_session_t* session = NULL;
    char *token = NULL, *c_stat = NULL;
    int have_authoriz = 0, have_error = 0, ret = 0;
    char buf[64];

    /* method struct must be initialized */
    if ( m == NULL ) {
	nbu_log_info( "method struct is NULL" );
	api_set_error( API_ERROR_METHOD_NOT_FOUND );
	ewf_fastcgi_display( "api/generic.xml" );
	return;
    }

    /* test if method exist */
    reqm = ewf_http_request_method(  );
    if ( reqm != m->request_method ) {
	nbu_log_info
	    ( "request method is wrong: got '%d', should be '%d'", reqm, m->request_method );
	api_set_error( API_ERROR_METHOD_NOT_FOUND );
	ewf_fastcgi_display( "api/generic.xml" );
	return;
    }

    /* action handler is defined ? */
    if ( m->action_handler == NULL ) {
	nbu_log_info( "action handler is NULL" );
	api_set_error( API_ERROR_METHOD_NOT_FOUND );
	ewf_fastcgi_display( "api/generic.xml" );
	return;
    }

    /* the box being upgrade ? */
        ret = nbd_autoconf_get("status", buf, sizeof(buf));
	if (ret == NBD_SUCCESS)
	{
		if(strncmp("downloading", buf, 11) == 0)
		{
			nbu_log_info( "The box is being upgrade ..." );
			api_set_error( API_ERROR_BOX_UPGRADING );
			ewf_fastcgi_display( "api/generic.xml" );
			ret = NBU_SUCCESS;
		}
	}
    
	/* if method is a POST method, parse POST arguments */
    if ( reqm == EWF_HTTP_REQUEST_METHOD_POST ) {
	ewf_fastcgi_parse(  );
    }

    /* check auth if method is private */
    if(m->access_policy == API_ACCESS_POLICY_PRIVATE)
    {
	    nbu_log_debug( "method '%s' is private !", name );
            
            /* maybe, api authentification is desactivated ? */
            if(nbd_nv_get("web_auth", buf, sizeof(buf)) == NBD_SUCCESS
               && nbu_string_matches(buf, "off") == NBU_STRING_EQUAL)
            {
                    nbu_log_debug( "api auth is desactivated" );
                    have_authoriz = 1;
            }
            else
            {
                    /* check token */
                    if(ewf_fastcgi_get_parameter("token", &token) != EWF_SUCCESS)
                    {
                            /* no token submit, return need auth ! */
                            api_set_error(API_ERROR_NEED_AUTH);
                    }
                    else
                    {
                            /* token is here, try to get session */
                            nbu_log_debug("Token is here ! %s", token);
                            if(ewf_session_get(token, &session) != EWF_SUCCESS)
                            {
                                    /* no session found ! */
                                    api_set_error(API_ERROR_NEED_AUTH);
                            }
                            else
                            {
                                    nbu_log_debug("Token is good !");
			    
                                    /* verify that session is valid and if the case, update it ! */
                                    if(ewf_session_check(session) != EWF_SUCCESS)
                                    {
                                            /* invalid session */
                                            api_set_error(API_ERROR_NEED_AUTH);
                                    }
                                    else
                                    {
                                            /* authentified ! */
                                            nbu_log_debug("User allowed to access private method");
                                            have_authoriz = 1;
                                    }
                            }
                    }
            }
    }
    else
    {
	    /* public method can be acceeded by anyone */
	    have_authoriz = 1;
    }
    
    if(!have_authoriz)
    {
	    have_error = 1;
    }
    else
    {
	    m->action_handler(  );
	    
	    /* check if an error has "throw" in the action method */
	    if((c_stat = hdf_get_value(cgi->hdf, "stat", NULL)) != NULL && nbu_string_compare(c_stat, "fail") == NBU_STRING_EQUAL)
	    {
		    have_error = 1;
	    }
	    else
	    {
		    if ( reqm == EWF_HTTP_REQUEST_METHOD_GET ) 
		    {
			    nbu_string_printf( buffer, sizeof buffer, "api/%s.%s", name, "xml" );
		    }
		    else 
		    {
			    /* display ok msg with generic xml template */
			    nbu_string_printf( buffer, sizeof buffer, "api/%s.%s", "generic", "xml" );
		    }
		    
		    api_set_value( "stat", "ok" );
	    }
    }
    
    if(have_error)
    {
	    /* display error with generic xml template */
	    nbu_string_printf( buffer, sizeof buffer, "api/%s.%s", "generic", "xml" );
    }

    ewf_fastcgi_display( buffer );

    return;
}

void api_set_error( api_error_t error )
{
	return api_set_custom_error(error_table[error].err_id,
				    error_table[error].err_msg);
}

void api_set_custom_error(const char *err_id, const char *err_msg)
{
	api_set_value( "stat", "fail" );

	nbu_log_debug( "%s", err_msg );
	api_set_value( "err.code", err_id );
	api_set_value( "err.msg", err_msg );
	
	return;
}

char *api_get_parameter( const char *name )
{
    char *val = NULL;

    if ( name == NULL ) {
	nbu_log_error( "param name is NULL" );
	return NULL;
    }

    nbu_string_printf( buffer, sizeof buffer, "Query.%s", name );
    val = hdf_get_value( cgi->hdf, buffer, NULL );
    nbu_log_debug( "%s: '%s'", name, val == NULL ? "NULL" : val );

    return val;
}

void api_set_value( const char *name, const char *value )
{
    char buf[64];

    if ( name == NULL ) {
	nbu_log_debug( "name of variable is NULL!" );
	return;
    }

    if ( value == NULL ) {
	nbu_log_debug( "value of '%s' is NULL!", name );
	return;
    }

    if ( nbu_string_printf( buf, sizeof buf, "%s", name ) == -1 ) {
	nbu_log_debug( "'%s' has been truncated!", name );
	return;
    }

    hdf_set_value( cgi->hdf, buf, value );
    nbu_log_debug( "%s: '%s'", buf, value );

    return;
}
