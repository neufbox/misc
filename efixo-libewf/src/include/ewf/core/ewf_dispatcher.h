#ifndef _EWF_DISPATCHER_H_
#define _EWF_DISPATCHER_H_

#include "ewf/ewf_page.h"
#include "ewf/core/ewf_request.h"

extern ewf_page_t *ewf_page_error;

/*
 * dispatch the request to right page
 */
extern void ewf_dispatcher_request(ewf_request_t *request);

/*
 * add page
 */
extern int ewf_dispatcher_registerpage(ewf_page_t *page);

/*
 * check consistency of the dispatcher
 */
extern int ewf_dispatcher_checkup(void);

#endif
