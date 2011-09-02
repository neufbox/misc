#include "api_common.h"
#include "ewf.h"
#include "nbu/nbu.h"

#ifdef DMALLOC
#define FINI_DMALLOC 1
#  include "dmalloc.h"
#endif

static void __attribute__ ( ( constructor ) ) __init( void )
{
    api_init(  );
}

int main( int argc, char **argv, char **envp )
{
#ifdef DMALLOC
	unsigned long mark;
#endif
	
	nbu_log_info("******************************************************");
	nbu_log_info("* API - Build-Date: " __DATE__ " " __TIME__ " - User: "__USER__" *");
	nbu_log_info("******************************************************");
	nbu_log_info("   \"Babes, bullets, bombs - Damn I love this job!\"");
	
	cgiwrap_init_std(argc, argv, envp);
	cgiwrap_init_emu( NULL, &ewf_fastcgi_read_cb, &ewf_fastcgi_writef_cb,
			  &ewf_fastcgi_write_cb, NULL, NULL, NULL );

	while ( ewf_fastcgi_accept(  ) == EWF_SUCCESS ) 
	{
	    
#ifdef DMALLOC
		/* get the current dmalloc position */
		mark = dmalloc_mark(  );
#endif
 		nbu_log_debug("------------------------------------------");
		
		api_begin(  );
		
		api_dispatch(  );
		
		api_end(  );

#ifdef DMALLOC
		/*
		 * log unfreed pointers that have been added to
		 * the heap since mark
		 */
		dmalloc_log_changed( mark, 1 /* log unfreed pointers */ ,
				     0 /* do not log freed pointers */ ,
				     1 /* log each pnt otherwise summary */  );
#endif
	}

	return 0;
}

static void __attribute__ ( ( destructor ) ) __fini( void )
{
    api_cleanup(  );
}
