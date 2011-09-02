#ifndef _EWF_CORE_H_
#define _EWF_CORE_H_

#include "ClearSilver.h"

/*! Global pointer to the CGI context.
 */
extern CGI *cgi;

/*
 * Initialize stuff on daemon startup.
 */
extern void ewf_core_init(void);

/*
 * Clean up stuff before shutting down the daemon.
 */
extern void ewf_core_cleanup(void);

/*
 * main function to start ewf
 */
extern int ewf_core_start(int argc, char **argv, char **envp);

#endif
