#include "ewf/ewf.h"

#include "ewf/core/ewf_config.h"
#include "ewf/core/ewf_matrix.h"

#include "nbu/string.h"

#include <stdlib.h>

ewf_config_env_t ewf_config_env = {
	.dir_vdocroot = "/var/www-vdocroot",
	.dir_webapproot = "/var/www",
};

static struct {
	char *name;
	char *value;
} conf_default_value[] = {
	{ "web_lang", "en", },
	{ "web_auth", "off", },
	{ "web_login", "admin", },
	{ "web_password", "", },
	{ "web_theme_uri", "local://default", },
};

void ewf_config_initenv(void)
{
	char *val = NULL;
        
	if((val = getenv("EWF_VDOCROOT")) != NULL)
	{
		ewf_config_env.dir_vdocroot = val;
	}

	if((val = getenv("EWF_WEBAPPROOT")) != NULL)
	{
		ewf_config_env.dir_webapproot = val;
	}

	nbu_log_debug("vdocroot dir = '%s'",
                      ewf_config_env.dir_vdocroot);
	nbu_log_debug("webapproot dir = '%s'",
                      ewf_config_env.dir_webapproot);
}

int ewf_config_set(const char *name, const char *value)
{
	int ret;
	
	if(ewf_matrix_ops.conf_set != NULL)
	{
		ret = ewf_matrix_ops.conf_set(name, value);
	}
	else
	{
		nbu_log_error("Unable to set conf, "
			      "define conf_set in ewf_matrix_ops");
		ret = EWF_ERROR;
	}

	return ret;
}

int ewf_config_get(const char *name, char *value, size_t value_size)
{
	size_t i;
	
	/* sanitize value */
	value[0] = '\0';
	
	if(ewf_matrix_ops.conf_get != NULL)
	{
		if(ewf_matrix_ops.conf_get(name, 
                                           value, 
                                           value_size) == EWF_SUCCESS)
                {
                        return EWF_SUCCESS;
                }
                else
                {
                        nbu_log_debug("ewf_matrix_ops.conf_get(%s) failed !",
                                      name);
                }
	}
        
        for(i = 0; i < NBU_ARRAY_SIZE(conf_default_value); i++)
        {
                if(nbu_string_compare(conf_default_value[i].name,
                                      name) == NBU_SUCCESS)
                {
                        nbu_log_debug("%s = '%s' (default value)",
                                      name, 
                                      conf_default_value[i].value);
				
                        nbu_string_copy(value,
                                        value_size,
                                        conf_default_value[i].value);
				
                        return EWF_SUCCESS;
                }
        }

        nbu_log_error("ewf_config_get(%s) failed !", name);
        return EWF_ERROR;
}

int ewf_config_get_int(const char *name)
{
	char buf[256];
	int val = -1;
	
	if(ewf_config_get(name, buf, sizeof(buf)) == EWF_SUCCESS)
	{
		errno = 0;
		val = strtol(buf, NULL, 10);
		if(errno != 0)
		{
			nbu_log_error("strtol '%s'='%s' failed: %s",
				      name, buf, strerror(errno));	
		}
	}
	else
	{
		nbu_log_error("Unable to get '%s' conf value",
			      name);
	}

	return val;
}
