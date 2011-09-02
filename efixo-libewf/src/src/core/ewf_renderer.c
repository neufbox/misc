#include "ewf/ewf.h"

#include "ewf/core/ewf_renderer.h"
#include "ewf/core/ewf_core.h"
#include "ewf/core/ewf_config.h"

static int force_raw_out = 0;
static int force_content_type = CONTENT_TYPE_AUTO;

void ewf_renderer(ewf_request_t *request, ewf_page_t *page)
{
	char buf[256];
	char *ext = NULL;
	NEOERR *err = NULL;
	
        if(force_raw_out)
        {
                /* do nothing */
                force_raw_out = 0;
                return;
        }

        if(force_content_type == CONTENT_TYPE_HTML)
        {
                nbu_log_debug("Force content type to HTML");
                force_content_type = CONTENT_TYPE_AUTO;
                hdf_set_value(cgi->hdf, 
                              "cgiout.ContentType", "text/html");
                ext = "html";
        }
        else if(force_content_type == CONTENT_TYPE_XML)
        {
                nbu_log_debug("Force content type to XML");
                force_content_type = CONTENT_TYPE_AUTO;
                hdf_set_value(cgi->hdf, "cgiout.ContentType", "text/xml");
		ext = "xml";
        }
        else
        {
                if(request->type == ajax_request)
                {
                        nbu_log_debug("Request type AJAX, so set content type to XML");
                        hdf_set_value(cgi->hdf, 
                                      "cgiout.ContentType", "text/xml");
                        ext = "xml";
                }
                else
                {
                        nbu_log_debug("set content type to HTML");
                        hdf_set_value(cgi->hdf, 
                                      "cgiout.ContentType", "text/html");
                        ext = "html";
                }
        }
        
	nbu_string_printf(buf, sizeof buf, 
			  "%s/fcgiroot/tpl%s.%s",
			  ewf_config_env.dir_webapproot,
			  page->name, ext);

	err = cgi_display(cgi, buf);

	if (err != NULL)
	{
#ifdef DEBUG
		cgi_neo_error(cgi, err);
#endif
		nerr_log_error(err);
		nerr_ignore(&err);
	}
}

void ewf_renderer_force_raw_output(void)
{
        force_raw_out = 1;
}

void ewf_renderer_force_content_type_output(int type)
{
        force_content_type = type;
}
