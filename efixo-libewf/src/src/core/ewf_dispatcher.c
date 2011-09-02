#include "ewf/ewf.h"
#include "ewf/ewf_page.h"

#include "ewf/core/ewf_core.h"
#include "ewf/core/ewf_dispatcher.h"
#include "ewf/core/ewf_controller.h"
#include "ewf/core/ewf_renderer.h"
#include "ewf/core/ewf_request.h"
#include "ewf/core/ewf_config.h"
#include "ewf/core/ewf_matrix.h"

#include "nbu/nbu.h"
#include "nbu/string.h"

#include <search.h>

/*************************/
/* public variables      */
/*************************/
ewf_page_t *ewf_page_error;

/*************************/
/* static variables      */
/*************************/
#define MAX_PAGES_NUM 100
static struct hsearch_data *htable_page = NULL;

/*************************/
/* static fcts           */
/*************************/
static void __attribute__((constructor)) __init3(void)
{
	htable_page = calloc(1, sizeof(struct hsearch_data));
	if (htable_page == NULL)
	{
		nbu_log_error("allocation failed for htable");
		exit(EXIT_FAILURE);
	}
		
	if (hcreate_r(MAX_PAGES_NUM, htable_page) == 0)
	{
		nbu_log_error("hash table creation failed");
		free(htable_page);
		exit(EXIT_FAILURE);
	}
}

static void __attribute__((destructor)) __fini3(void)
{
	free(htable_page);
	htable_page = NULL;
}

static ewf_page_t* ewf_dispatcher_getpage(ewf_request_t *request)
{
	char pgname[128];
	char *c = NULL;

	ENTRY e, *ep;
	ewf_page_t *p = NULL;
	
	if (request->uri == NULL)
	{
		return NULL;
	}
	
	/* index page wanted ? */
	if (request->uri[0] == '\0'
	    || (request->uri[0] == '/' && request->uri[1] == '\0'))
	{
                nbu_string_copy(pgname, sizeof(pgname), URI_PAGE_INDEX);
	}
	else
	{
		nbu_string_copy(pgname, sizeof(pgname), request->uri);
	}

	/* remove query string after ? */
	c = pgname;
	while (*c != '\0')
	{
		if ((*c == '?'))
		{
			*c = '\0';
			break;
		}

		++c;
	}
	
	e.key = pgname;
	
	if (hsearch_r(e, FIND, &ep, htable_page) != 0)
	{
		p = ep->data;
                if(request->type != ajax_request)
                {
                        nbu_log_info("page '%s' (uri=%s) found",
                                     pgname, request->uri);
                }
	}
	else
	{
		nbu_log_info("page '%s' (uri=%s) not found", 
			     pgname, request->uri);
	}
	
	return p;
}

static void ewf_dispatcher_error(ewf_request_t *request, int errnum)
{
	nbu_log_notice("%s - %d", request->uri, errnum);
	
	hdf_set_int_value(cgi->hdf, "Var.error.Value", errnum);
	
	if(errnum == HTTP_ERROR_404)
	{
		hdf_set_value(cgi->hdf, "Var.error.uri.Value", 
			      request->uri != NULL ? request->uri : "NULL");
	}

	return ewf_controller(request, ewf_page_error);
}

/*************************/
/* public fcts           */
/*************************/
void ewf_dispatcher_request(ewf_request_t *request)
{
	ewf_page_t *page = NULL;
        
        /* matrix pre dispatch */
	if(ewf_matrix_ops.pre_dispatch != NULL)
	{
		ewf_matrix_ops.pre_dispatch(request);
	}
	
	if((page = ewf_dispatcher_getpage(request)) != NULL)
	{
		ewf_controller(request, page);
	}
	else
	{
		nbu_log_error("No found page linked with this uri '%s'.",
			      request->uri);
		ewf_dispatcher_error(request, HTTP_ERROR_404);
	}
	
	return;
}

int ewf_dispatcher_registerpage(ewf_page_t *page)
{
	ENTRY e, *ep = NULL;
	int ret = EWF_ERROR;
	
	e.key = page->name;
	e.data = (void *) page;

	if (hsearch_r(e, ENTER, &ep, htable_page) != 0)
	{
		nbu_log_debug("page '%32s' initialized", 
			      page->name);
		ret = EWF_SUCCESS;
	}
	else
	{
		nbu_log_error("hash table is full burp!, page '%32s'"
			      "not added", page->name);
	}
	
	return ret;
}

int ewf_dispatcher_checkup(void)
{
	/*
	 * test existence of page error
	 */

        char tmp[128];
        ewf_request_t request;
        
        nbu_string_copy(tmp, sizeof(tmp), URI_PAGE_ERROR);
        request.uri = tmp;

	if((ewf_page_error = ewf_dispatcher_getpage(&request)) == NULL
	   || ewf_page_error->display_handler == NULL
	   || ewf_page_error->ajax_handler == NULL)
	{
		nbu_log_error("EWF requires " URI_PAGE_ERROR " page with "
                              "display and ajax handler to work properly");
		return EWF_ERROR;
	}

	return EWF_SUCCESS;
}
