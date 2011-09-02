#include <unistd.h>
#include "ewf.h"
#include "nbu/nbu.h"

int ewf_fastcgi_read_cb( void *ptr, char *data, int size )
{
	NBU_UNUSED(ptr);
	
	return FCGI_fread( data, sizeof( char ), size, FCGI_stdin );
}

int ewf_fastcgi_writef_cb( void *ptr, const char *format, va_list ap )
{
	NBU_UNUSED(ptr);
	
	FCGI_vprintf( format, ap );

	return 0;
}

int ewf_fastcgi_write_cb( void *ptr, const char *data, int size )
{
	NBU_UNUSED(ptr);
	
	/* error here */
	return FCGI_fwrite( ( void * ) data, sizeof( char ), size, FCGI_stdout );
}

void ewf_fastcgi_set_content_type( const char *type )
{
	api_set_value( "cgiout.ContentType", type );

	return;
}

void ewf_fastcgi_parse( void )
{
    NEOERR *err;

    err = cgi_parse( cgi );
    nerr_ignore( &err );
    nbu_log_info( "POST request method" );

    return;
}

int ewf_fastcgi_accept( void )
{
    if ( FCGI_Accept(  ) < 0 ) {
	return EWF_ERROR;
    }

    return EWF_SUCCESS;
}

void ewf_fastcgi_display( const char *name )
{
    NEOERR *err;

    /*printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"); */

    if ( access( name, F_OK | R_OK ) != 0 ) {
	nbu_log_debug( "cannot read CST file: '%s'", name );
    }
    else {
	nbu_log_debug( "read CST file: '%s'", name );
    }

    err = cgi_display( cgi, name );

#ifdef DEBUG
    if ( err != NULL ) {
	nbu_log_debug( "error in cgi_display!" );
	cgi_neo_error( cgi, err );
    }
#endif

    nerr_ignore( &err );

    return;
}

int ewf_fastcgi_get_parameter( const char *name, char **value )
{
    char buf[64];

    if ( name == NULL ) 
    {
	nbu_log_error( "param name is NULL" );
	return EWF_ERROR;
    }

    nbu_string_printf( buf, sizeof buf, "Query.%s", name );
    *value = hdf_get_valuef( cgi->hdf, buf );

    if ( *value == NULL ) {
	return EWF_ERROR;
    }

    return EWF_SUCCESS;
}
