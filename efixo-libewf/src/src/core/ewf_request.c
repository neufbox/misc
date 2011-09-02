#include "ewf/ewf.h"

#include "ewf/core/ewf_core.h"
#include "ewf/core/ewf_request.h"

#include "nbu/nbu.h"
#include "nbu/string.h"
#include "nbu/log.h"

/*************************/
/* static functions      */
/*************************/
static int ewf_request_type(void)
{
	char *val = NULL;

	val = hdf_get_valuef(cgi->hdf, "HTTP.XRequestedHandler");
	if (val != NULL
	    && nbu_string_matches(val, "ajax") == NBU_STRING_EQUAL)
	{
		/* ajax request */
		nbu_log_debug("request type: AJAX");
		
		return ajax_request;
	}
	
	val = hdf_get_valuef(cgi->hdf, "CGI.RequestMethod");
	if (val != NULL
	    && nbu_string_matches(val, "POST") == NBU_STRING_EQUAL)
	{
		/* form request */
		nbu_log_debug("request type: FORM");
		
		return form_request;
	}

	/* otherwise display request */
	nbu_log_debug("request type: DISPLAY");
	
	return display_request;
}

/* static void ewf_request_get_pgrequest(char *buf, size_t buf_size) */
/* { */
/*         char *scriptname = hdf_get_value(cgi->hdf, "CGI.ScriptName", "/"); */
/*         char *c = NULL; */

/*         nbu_string_copy(buf, buf_size, scriptname); */

/*         c = buf; */
/* 	while (*c != '\0') */
/* 	{ */
/*                 if ((*c < '/')  */
/*                     || (*c > '9' && *c < 'A') */
/*                     || (*c > 'Z' && *c < '_') */
/*                     || (*c > '_' && *c < 'a') */
/*                     || (*c > 'z')) */
/* 		{ */
/* 			*c = '\0'; */
/* 			break; */
/* 		} */

/* 		++c; */
/* 	} */
/* } */

/*************************/
/* public functions      */
/*************************/
void ewf_request_init(ewf_request_t *request)
{
	NEOERR *err = NULL;
	
	/* fill cgi struct */
	err = cgi_init(&cgi, NULL);

	if (err != NULL)
	{
#ifdef DEBUG
		cgi_neo_error(cgi, err);
#endif
		nerr_log_error(err);
		nerr_ignore(&err);
	}

	request->type = ewf_request_type();
	request->uri = hdf_get_value(cgi->hdf, "CGI.RequestURI", "/");
}

void ewf_request_end(ewf_request_t *request)
{
	NBU_UNUSED(request);
	
	cgi_destroy(&cgi);
}
