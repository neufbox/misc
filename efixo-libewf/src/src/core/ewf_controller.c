#include "ewf/ewf.h"
#include "ewf/ewf_utils.h"
#include "ewf/ewf_hdf.h"

#include "ewf/core/ewf_core.h"
#include "ewf/core/ewf_controller.h"
#include "ewf/core/ewf_dispatcher.h"
#include "ewf/core/ewf_renderer.h"
#include "ewf/core/ewf_matrix.h"
#include "ewf/core/ewf_smith.h"
#include "ewf/core/ewf_config.h"

static void ewf_controller_error(ewf_request_t *request, 
				 ewf_page_t **page, 
				 int errnum)
{
	char buf[128];

        nbu_log_debug("error = %d", errnum);
        
	if(errnum == APP_ERROR_112)
	{
		if(request->type == ajax_request)
		{
			hdf_set_value(cgi->hdf, 
				      "Var.error.method.Value", "ajax");
		}
		else
		{
			hdf_set_value(cgi->hdf, 
				      "Var.error.method.Value", "display");
		}
	}
	else if(errnum == APP_ERROR_115)
	{
		if(request->type != ajax_request)
		{
			/* redirect to login page */
			nbu_string_printf(buf, sizeof buf, 
					  URI_PAGE_LOGIN "?page_ref=%s", 
					  (*page)->name);
			nbu_log_debug("redirect= %s", buf);
			ewf_utils_redirect(buf);
		}
	}
	
	hdf_set_int_value(cgi->hdf, "Var.error.Value", errnum);
        *page = ewf_page_error;
}

/*************************/
/* public fcts           */
/*************************/
void ewf_controller(ewf_request_t *request, ewf_page_t *page)
{
	NEOERR *err;
	char buf[256];
	void (*disp_handler)(void) = NULL;
	int ret;
	char lang[8];

	if((ret = ewf_smith_access_control(page)) >= EWF_SMITH_CTL_SUCCESS)
	{
		if(ret == EWF_SMITH_CTL_SUCCESS
		   || ret == EWF_SMITH_CTL_AUTH_OFF) 
		{
			if(ret == EWF_SMITH_CTL_SUCCESS)
			{
				hdf_set_value(cgi->hdf, 
					      "User.authenticated", "1");
			}
			else
			{
				hdf_set_value(cgi->hdf, 
					      "User.authenticated", "-1");
			}
		
			if(nbu_string_matches(page->name, URI_PAGE_LOGIN) 
			   == NBU_STRING_EQUAL)
			{
				/* user want to auth but security is off
				 * redirect it */
				ewf_utils_redirect(URI_PAGE_INDEX);
				return;
			}
		}

                /* load regex hdf file */
                nbu_string_printf(buf, sizeof buf,
                                  "rgx/%s.hdf", page->name);
                ewf_hdf_read_file(buf);

		/* find the correct handler */
		if (request->type == ajax_request)
		{
			if (page->ajax_handler != NULL)
			{
				nbu_log_debug("page '%s', ajax handler "
					      "not NULL", page->name);
						
				disp_handler = page->ajax_handler;
			}
			else
			{
				nbu_log_error("page '%s', ajax handler "
					      "is NULL!", page->name);
			}
		}
		else
		{
			if (request->type == form_request)
			{
				if (page->form_handler != NULL)
				{
					nbu_log_debug("page '%s', form "
						      "handler not NULL", 
						      page->name);
							
					err = cgi_parse(cgi);

					if (err != NULL)
					{
#ifdef DEBUG
						cgi_neo_error(cgi, err);
#endif
						nerr_log_error(err);
						nerr_ignore(&err);
					}
							
					page->form_handler();

					if(hdf_get_int_value(cgi->hdf, 
							     "Form.Status", 
							     0) == -1)
					{
						/* An error has occurred on 
						 * form handler, copy all query
						 * variable to Var subtree */
						nbu_log_debug("Form.Status != 0,"
							      "rewrite cgi post "
							      "value !");
						ewf_hdf_copy_cgi_query_tree();
					}
				}
				else
				{
					nbu_log_notice("page '%s', form handler "
                                                       "is NULL!", page->name);
				}
			}
					
			if (page->display_handler != NULL)
			{
				nbu_log_debug("page '%s', display handler not "
					      "NULL", page->name);
						
				disp_handler = page->display_handler;
			}
			else
			{
				nbu_log_debug("page '%s', display handler is "
					      "NULL!", page->name);
			}
		}
		
		/* call the "display" handler */
		if(disp_handler != NULL)
		{
			disp_handler();
		}
		else
		{
			ewf_controller_error(request, &page, APP_ERROR_112);
		}
	}
	else
	{ /* smith failed */
		hdf_set_value(cgi->hdf, "User.authenticated", "0");
		
		/* need authentification */
		ewf_controller_error(request, &page, APP_ERROR_115);
	}
	
	/* matrix pre render */
	if(ewf_matrix_ops.pre_render != NULL)
	{
		ewf_matrix_ops.pre_render(request, page);
	}
	
        /* load text */
        ewf_config_get("web_lang", lang, sizeof(lang));
	
        nbu_string_printf(buf, sizeof buf,
                          "lng/%s/text.hdf", 
                          lang);
        ewf_hdf_read_file(buf);
		
        nbu_string_printf(buf, sizeof buf, 
                          "lng/%s/%s.hdf", 
                          lang, page->name);
        ewf_hdf_read_file(buf);
	
	/* render the page */
	ewf_renderer(request, page);
	
	return;
}

