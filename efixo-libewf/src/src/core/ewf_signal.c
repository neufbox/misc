#include "ewf/ewf.h"

#include "ewf/core/ewf_signal.h"
#include "ewf/core/ewf_core.h"

#include <stdlib.h>
#include <signal.h>

#include "nbu/log.h"

/* action bits */
static volatile int reg_sig_action;

void ewf_signal_handler(int sn, siginfo_t *si, void *se)
{
	NBU_UNUSED(se);
	
	switch (sn)
	{
	case SIGSEGV:
	case SIGBUS:
		nbu_log_info("              ");
		nbu_log_info("   ,/       \\,    ");
		nbu_log_info("  //         \\\\  ");
		nbu_log_info(" ((__,-\"\"\"-,__))");
		nbu_log_info("  `--)~   ~(--`    ");
		nbu_log_info(" .-'(       )`-,   ");
		nbu_log_info(" `~~`X)   (X`~~`   ");
		nbu_log_info("     |     |       ");
		nbu_log_info("     |     |    |-----------| ");
		nbu_log_info("     |     |    | MEUHHHH ! | ");
		nbu_log_info("     |     |    |-----------| ");
		nbu_log_info("     |     |      /    ");
		nbu_log_info("     (6___6)    _/  ");
		nbu_log_info("      `---`        ");
		nbu_log_info("");
		if(sn == SIGBUS)
		{
			nbu_log_info(" %%> BUS !");
		}
		else
		{
			nbu_log_info(" %%> SEGV %s", 
				     (si->si_code == SEGV_MAPERR) ? "Address not mapped to object" : "Invalid permissions for mapped object");
			nbu_log_info(" %%> SEGV addr: %x", si->si_addr);
			nbu_log_info(" %%> LIBEWF version " LIBEWF_VERSION);
		}
		
		exit(EXIT_FAILURE);
		break;
		
	case SIGTERM:
		nbu_log_info("SIGTERM received, exit \"cleanly\"");
		exit(EXIT_SUCCESS);
		break;
		
	case SIGINT:
		nbu_log_info("SIGINT received, exit \"cleanly\"");
		exit(EXIT_SUCCESS);
		break;
		
	case SIGHUP:
		nbu_log_info("SIGHUP received, set reload conf trigger");
		ewf_signal_set_action(SIG_ACTION_RELOAD);
		break;
		
	default:
		nbu_log_notice("signal not handled");
		break;
	}
}

void ewf_signal_setup(void)
{
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = ewf_signal_handler;
	
	if (sigemptyset(&sa.sa_mask) != 0) 
	{
		nbu_log_error("sigemptyset failed");
	}
	
	if (sigaddset(&sa.sa_mask, SIGTERM)) 
	{
		nbu_log_error("sigaddset SIGTERM failed");
	}
	
	if (sigaddset(&sa.sa_mask, SIGINT)) 
	{
		nbu_log_error("sigaddset SIGINT failed");
	}
	
	if (sigaddset(&sa.sa_mask, SIGHUP)) 
	{
		nbu_log_error("sigaddset SIGHUP failed");
	}
	
	if (sigaction(SIGSEGV, &sa, NULL) != 0)
	{
		nbu_log_error("sigaction with SIGSEGV failed");
	}
	
	if (sigaction(SIGTERM, &sa, NULL) != 0)
	{
		nbu_log_error("sigaction with SIGTERM failed");
	}
	
	if (sigaction(SIGHUP, &sa, NULL) != 0)
	{
		nbu_log_error("sigaction with SIGHUP failed");
	}
}

void ewf_signal_set_action(int action)
{
	reg_sig_action |= action;
}

void ewf_signal_clear_action(int action)
{
	reg_sig_action &= ~action;
}

int ewf_signal_test_action(int action)
{
	return ((reg_sig_action & action) == action);
}
