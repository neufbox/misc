#ifndef _API_SIGNAL_H_
#define _API_SIGNAL_H_

#include <signal.h>

void api_signal_handler(int sn, siginfo_t *si, void *se);
void api_signal_setup(void);

#endif /* _API_SIGNAL_H_ */

