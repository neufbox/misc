#include "ewf/ewf.h"
#include "ewf/ewf_utils.h"

#include "ewf/core/ewf_core.h"

#include "pcre.h"

#include "nbu/string.h"
#include "nbu/log.h"

const char *ewf_utils_client_ip(void)
{
	return hdf_get_valuef(cgi->hdf, "CGI.RemoteAddress");
}

const char *ewf_utils_client_referer(void)
{
	return hdf_get_valuef(cgi->hdf, "HTTP.Referer");
}

void ewf_utils_redirect(const char *uri)
{
	cgi_redirect(cgi, "%s", uri);
}

FILE *ewf_utils_filehandle(const char *name)
{
	return cgi_filehandle(cgi, name);
}

int ewf_utils_check_referer(const char *page)
{
	char *value = NULL, *value_strip = NULL;
	char buf[64 + 1];
	
	int ret = EWF_ERROR;
	
	value = hdf_get_valuef(cgi->hdf, "CGI.ServerName");
	nbu_string_printf(buf, sizeof buf, "http://%s/", value);
  
	value = hdf_get_valuef(cgi->hdf, "HTTP.Referer");
	value_strip = value;
	while (*value_strip != '\0')
	{
		if ((*value_strip < '0')
		    || (*value_strip > '9' && *value_strip < '_')
		    || (*value_strip > '_' && *value_strip < 'a')
		    || (*value_strip > 'z'))
		{
			break;
		}
		
		++value_strip;
	}
	
	if (strncmp(buf, value, value_strip-value) == 0)
	{
		ret = EWF_SUCCESS;
	}
	
	return ret;
}

int ewf_utils_pcre_validate(const char *value, const char *pattern)
{
	const char *err;
	pcre *re;
	int erroffset;
	int rc;
	int ret;

	if ((value == NULL) || (pattern == NULL))
	{
		nbu_log_error("Validate FAILED (NULL component) !", 
			      value != NULL ? value : "NULL",
			      pattern != NULL ? pattern : "NULL");
		
		return EWF_ERROR;
	}

	re = pcre_compile(pattern, 0, &err, &erroffset, NULL);
	if (re == NULL)
	{
		return EWF_ERROR;
	}

	rc = pcre_exec(re, NULL, value, strlen(value), 0, 0, NULL, 0);
	pcre_free(re);

	ret = (rc == 0) ? EWF_SUCCESS : EWF_ERROR;

	nbu_log_debug("Validate '%s' => %s", value, (ret == EWF_SUCCESS) ? "valid" : "not valid!");

	return ret;
}
