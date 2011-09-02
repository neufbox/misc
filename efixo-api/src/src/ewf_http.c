#include <string.h>
#include "ewf.h"

ewf_http_request_method_t ewf_http_request_method( void )
{
    char *method = NULL;

    method = hdf_get_valuef( cgi->hdf, "CGI.RequestMethod" );
    if ( method == NULL ) {
	return EWF_HTTP_REQUEST_METHOD_UNKNOWN;
    }

    if ( ( method[3] == '\0' )
	 && ( strncmp( "GET", method, 3 ) == 0 ) ) {
	return EWF_HTTP_REQUEST_METHOD_GET;
    }
    else if ( ( method[4] == '\0' )
	      && ( strncmp( "POST", method, 4 ) == 0 ) ) {
	return EWF_HTTP_REQUEST_METHOD_POST;
    }
    else if ( ( method[3] == '\0' )
	      && ( strncmp( "PUT", method, 3 ) == 0 ) ) {
	return EWF_HTTP_REQUEST_METHOD_PUT;
    }
    else if ( ( method[6] == '\0' )
	      && ( strncmp( "DELETE", method, 6 ) == 0 ) ) {
	return EWF_HTTP_REQUEST_METHOD_DELETE;
    }

    return EWF_HTTP_REQUEST_METHOD_UNKNOWN;
}

char *ewf_http_request_uri( void )
{
    return hdf_get_valuef( cgi->hdf, "CGI.RequestURI" );
}
