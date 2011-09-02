#include <stdlib.h>
#include <signal.h>
#include <ucontext.h>
#include "api_common.h"
#include "api_signal.h"

#include "nbu/nbu.h"

void api_signal_handler( int sn, siginfo_t * si, void *se )
{
/* #ifdef NB4
	ucontext_t *uc = (ucontext_t *) se;
	int i;
#endif */
	NBU_UNUSED(se);

    switch ( sn ) {
    case SIGSEGV:
	nbu_log_info( "SIGSEGV %s",
		      ( si->si_code ==
			SEGV_MAPERR ) ? "Address not mapped to object" :
		      "Invalid permissions for mapped object" );
	/*
	   #ifdef NB4
	   for (i = 0; i < 36; i++)
	   {
	   nbu_log_info("SIGSEGV %d: 0x%08lX", i, uc->uc_mcontext.gpregs[i]);
	   }
	   nbu_log_info("SIGSEGV @0x%08lX, epc=0x%08lX, ra=0x%08lX", si->si_addr, uc->uc_mcontext.gpregs[CTX_EPC], uc->uc_mcontext.gpregs[CTX_RA]);
	   nbu_log_info("SIGSEGV received, exit now");
	   #endif
	 */
	/* cleanup is handled by destructor */
	//api_cleanup(  );
	exit( EXIT_FAILURE );
	break;

    case SIGTERM:
    case SIGINT:
	/* cleanup is handled by destructor */
	//api_cleanup(  );
	nbu_log_info( "SIGTERM received, exit cleanly" );
	exit( EXIT_SUCCESS );
	break;

    case SIGUSR1:
	nbu_log_info( "SIGUSR1 received" );
	break;

    default:
	nbu_log_info( "signal not handled" );
	break;
    }
}

void api_signal_setup( void )
{
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = api_signal_handler;

    if ( sigaction( SIGSEGV, &sa, NULL ) != 0 ) {
	nbu_log_error( "sigaction with SIGSEGV failed" );
    }

    if ( sigaction( SIGTERM, &sa, NULL ) != 0 ) {
	nbu_log_error( "sigaction with SIGTERM failed" );
    }

    if ( sigaction( SIGUSR1, &sa, NULL ) != 0 ) {
	nbu_log_error( "sigaction with SIGUSR1 failed" );
    }
}
