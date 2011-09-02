#ifndef _EWF_SIGNAL_H_
#define _EWF_SIGNAL_H_

#include <signal.h>

/*
 * action define
 */ 
#define SIG_ACTION_RELOAD 0x01

void ewf_signal_handler(int sn, siginfo_t *si, void *se);
void ewf_signal_setup(void);

void ewf_signal_set_action(int action);
void ewf_signal_clear_action(int action);
int ewf_signal_test_action(int action);

#endif
