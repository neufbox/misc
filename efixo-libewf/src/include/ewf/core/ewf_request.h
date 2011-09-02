#ifndef _EWF_REQUEST_H_
#define _EWF_REQUEST_H_

enum request_type {
	display_request = 0,
	form_request,
	ajax_request,
};

typedef struct ewf_request_t {
	int type;
	char *uri;
} ewf_request_t;

/*! Initialize stuff before processing the new request.
 */
extern void ewf_request_init(ewf_request_t *ewf_request);

/*! Clean up stuff after processing the request.
 */
extern void ewf_request_end(ewf_request_t *ewf_request);

#endif
