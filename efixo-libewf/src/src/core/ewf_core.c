#include "ewf/ewf.h"
#include "ewf/core/ewf_matrix.h"

#include "ewf/core/ewf_core.h"
#include "ewf/core/ewf_fcgi.h"
#include "ewf/core/ewf_signal.h"
#include "ewf/core/ewf_request.h"
#include "ewf/core/ewf_wrapper.h"
#include "ewf/core/ewf_dispatcher.h"
#include "ewf/core/ewf_config.h"
#include "ewf/core/ewf_theme.h"

#include "nbu/log.h"

#include <unistd.h>
#include <stdio.h>

#include "fcgios.h" /* for OS_LibShutdown */

/*
 * must be inserted at the bottom of the include files
 * as to not conflict with other includes */
#ifdef DMALLOC
#define FINI_DMALLOC 0
#include "dmalloc.h"
#endif

/*************************/
/* static vars and funcs */
/*************************/
CGI *cgi;

static void ewf_core_reload(void)
{
	/* reload theme */
	ewf_theme_load();
}

/*************************/
/* public fcts           */
/*************************/
void ewf_core_init(void)
{
	pid_t pid;
	FILE *f_pid = NULL;
	
	/* register signal handler and open log */
	ewf_signal_setup();
	nbu_log_open("fastcgi");
	nbu_log_debug("***************** libewf log started *****************");
	
	/* register pid */
	pid = getpid();

	f_pid = fopen("/var/run/fastcgi.pid", "w");
	if(f_pid != NULL)
	{
		fprintf(f_pid, "%d\n", pid);
		fclose(f_pid);
	}
	else
	{
		nbu_log_error("Unable to write pid !");
	}

#ifdef DMALLOC
	dmalloc_debug_setup("log-stats,log-non-free,check-fence,"
			    "log=/tmp/fastcgi.%p");
#endif
	/* init config_env */
	ewf_config_initenv();

        /* matrix core ext init */
	nbu_log_debug("matrix_ops core_ext_init = %p",
		      ewf_matrix_ops.core_ext_init);
	
	if(ewf_matrix_ops.core_ext_init != NULL)
	{
		nbu_log_debug("Call matrix_ops core_ext_init");
		if(ewf_matrix_ops.core_ext_init() != EWF_SUCCESS)
		{
			nbu_log_error("core ext init failed, exit");
			exit(EXIT_FAILURE);
		}
	}
	
        /* load theme */
        ewf_theme_load();

	nbu_log_debug("ewf core initialized");
}

void ewf_core_cleanup(void)
{
        /* matrix core ext cleanup */
	if(ewf_matrix_ops.core_ext_cleanup != NULL)
	{
		nbu_log_debug("Call matrix_ops core_ext_cleanup");
		ewf_matrix_ops.core_ext_cleanup();
	}
        
	/* free the memory used by the FastCGI library */
	OS_LibShutdown();

#ifdef DMALLOC
	dmalloc_shutdown();
#endif

	nbu_log_debug("ewf core cleaned");
	
	nbu_log_close();
}

int ewf_core_start(int argc, char **argv, char **envp)
{
	ewf_request_t request;
	sigset_t sigblocked_set, sigrestore_set;
	
	nbu_log_info("******************************************************");
	nbu_log_info("* EWF - Build-Date: " __DATE__ " " __TIME__ " *");
	nbu_log_info("******************************************************");
	nbu_log_info("        \"Boooorn to be wiiiiild!\"");

	/* check consistency */
	if(ewf_dispatcher_checkup() != EWF_SUCCESS)
	{
		nbu_log_error("Checkup failed ! exit.");
		return EXIT_FAILURE;
	}

	/* set signal list */
	sigemptyset(&sigrestore_set);
	sigemptyset(&sigblocked_set);
	sigaddset(&sigblocked_set, SIGHUP);
	
	cgiwrap_init_std(argc, argv, envp);
	cgiwrap_init_emu(NULL, 
			 &ewf_wrapper_read_cb, 
			 &ewf_wrapper_writef_cb, 
			 &ewf_wrapper_write_cb, 
			 &ewf_wrapper_getenv_cb, 
			 &ewf_wrapper_putenv_cb, 
			 &ewf_wrapper_iterenv_cb);
        
        nbu_log_info("* User web version: %s", ewf_matrix_vars.version);
        
	while (FCGX_Accept(&fcgx_in, &fcgx_out, &fcgx_err, &fcgx_envp) >= 0)
	{
		sigprocmask( SIG_SETMASK, &sigblocked_set, NULL );
		
		/* have received a reload action ? */
		if(ewf_signal_test_action(SIG_ACTION_RELOAD))
		{
			nbu_log_info("SIG_ACTION_RELOAD set, reload conf !");
			
			ewf_core_reload();
			
			ewf_signal_clear_action(SIG_ACTION_RELOAD);
		}
		
#ifdef DMALLOC
		unsigned long mark;
		/* get the current dmalloc position */
		mark = dmalloc_mark();
#endif
 		nbu_log_debug("------------------------------------------");
		
		ewf_request_init(&request);
		
#ifdef HDFDEBUG
		hdf_set_value(cgi->hdf, "Config.DebugEnabled", "1");
		hdf_set_value(cgi->hdf, "Query.debug", "foobar");
		hdf_set_value(cgi->hdf, "Config.DebugPassword", "foobar");
#endif
                hdf_set_value(cgi->hdf, "Config.TimeFooter", "0");
                
		ewf_dispatcher_request(&request);

		ewf_request_end(&request);

#ifdef DMALLOC
		/*
		* log unfreed pointers that have been added to
		* the heap since mark
		*/
		dmalloc_log_changed(mark,
				    1 /* log unfreed pointers */,
				    0 /* do not log freed pointers */,
				    1 /* log each pnt otherwise summary */);
#endif
		sigprocmask( SIG_SETMASK, &sigrestore_set, NULL );
	}

	return 0;
}

