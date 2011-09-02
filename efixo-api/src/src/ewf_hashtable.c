#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <stdlib.h>
#include <search.h>
#include "ewf.h"

/* http://csourcesearch.net/package/uclibc/0.9.27/uClibc-0.9.27/libc/misc/search/hsearch_r.c */

int ewf_hashtable_create( ewf_hashtable_t ** htab, size_t size )
{
    /* From the man page:
     * "The struct it points to must be zeroed before the first call to hcreate_r()."
     */
    *htab = calloc( 1, sizeof( struct hsearch_data ) );

    if ( *htab == NULL ) {
	nbu_log_error( "hash table allocation failed" );
	return EWF_ERROR;
    }

    if ( hcreate_r( size, *htab ) == 0 ) {
	nbu_log_error( "hash table creation failed" );
	return EWF_ERROR;
    }

    return EWF_SUCCESS;
}

int ewf_hashtable_destroy( ewf_hashtable_t ** htab )
{
    if ( *htab == NULL ) {
	nbu_log_error( "hash table pointer is NULL" );
	return EWF_ERROR;
    }

    hdestroy_r( *htab );
    free( *htab );

    nbu_log_debug( "hash table has been destroyed" );

    return EWF_SUCCESS;
}

int ewf_hashtable_insert( ewf_hashtable_t ** htab, const char *key, void **data )
{
    ENTRY e, *ep = NULL;

    e.key = ( char * ) key;
    e.data = *data;

    if ( hsearch_r( e, ENTER, &ep, *htab ) == 0 ) {
	nbu_log_error( "hash table is full" );
	return EWF_ERROR;
    }

    return EWF_SUCCESS;
}

int ewf_hashtable_find( ewf_hashtable_t ** htab, const char *key, void **data )
{
    ENTRY e, *ep = NULL;

    e.key = ( char * ) key;
    hsearch_r( e, FIND, &ep, *htab );

    if ( ep == NULL ) {
	return EWF_ERROR;
    }

    *data = ep->data;

    return EWF_SUCCESS;
}
