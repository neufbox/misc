#include "ewf/ewf.h"
#include "ewf/ewf_hdf.h"
#include "ewf/ewf_utils.h"

#include "ewf/core/ewf_core.h"
#include "ewf/core/ewf_secure.h"

#include <pcre.h>
#include <string.h>
#include <errno.h>

#include "nbu/nbu.h"
#include "nbu/log.h"

static char _internal_buf[256];
static char _internal_buf2[512];

static int _ewf_hdf_vset(const char *var_name, const char *fmt, va_list args, int overwrite)
{
	char *val = NULL;
	int ret = EWF_SUCCESS;
	
	if(!var_name || !fmt)
	{
		nbu_log_error("Set '%s' = '%s' FAILED (NULL component) !", 
			      var_name != NULL ? var_name : "NULL",
			      fmt != NULL ? fmt : "NULL");
		
		return EWF_ERROR;
	}
	
	if(nbu_string_printf(_internal_buf, sizeof _internal_buf, "Var.%s.Value", var_name) == NBU_ERROR)
	{
		nbu_log_error("Var '%s' can be incoherent or truncated !", _internal_buf);
		ret = EWF_ERROR;
	}
	
	if( !overwrite && (val = hdf_get_value(cgi->hdf, _internal_buf, NULL)) != NULL )
	{
		/* hdf var has already an value ! */
		nbu_log_debug("Var '%s' (='%s') already exist, don't overwrite it !", _internal_buf, val);
		return EWF_ERROR;
	}

	if(nbu_string_vprintf(_internal_buf2, sizeof _internal_buf2, fmt, args) == NBU_ERROR)
	{
		nbu_log_error("Value of '%s' can be incoherent or truncated !", _internal_buf);
		ret = EWF_ERROR;
	}
	
	hdf_set_value(cgi->hdf, _internal_buf, _internal_buf2);
	
	nbu_log_debug("Set '%s' = '%s'", _internal_buf, _internal_buf2);

	return ret;
}

static int _ewf_hdf_set(const char *var_name, const char *str_value, int overwrite)
{
	char *val = NULL;
	int ret = EWF_SUCCESS;
	
	if(!var_name || !str_value)
	{
		nbu_log_error("Set '%s' = '%s' FAILED (NULL component) !", 
			      var_name != NULL ? var_name : "NULL",
			      str_value != NULL ? str_value : "NULL");
		
		return EWF_ERROR;
	}
	
	if(nbu_string_printf(_internal_buf, sizeof _internal_buf, "Var.%s.Value", var_name) == NBU_ERROR)
	{
		nbu_log_error("Var '%s' can be incoherent or truncated !", _internal_buf);
		ret = EWF_ERROR;
	}
	
	if( !overwrite && (val = hdf_get_value(cgi->hdf, _internal_buf, NULL)) != NULL )
	{
		/* hdf var has already an value ! */
		nbu_log_debug("Var '%s' (='%s') already exist, don't overwrite it !", _internal_buf, val);
		return EWF_ERROR;
	}
	
	hdf_set_value(cgi->hdf, _internal_buf, str_value);
	
	nbu_log_debug("Set '%s' = '%s'", _internal_buf, str_value);

	return ret;
}

static int ewf_hdf_validate(const char *var_name, const char *value)
{
	char *pattern = NULL;
        char buf[128];
	
        if(value == NULL)
        {
                nbu_log_error("%s = NULL !", var_name);
                return EWF_ERROR;
        }

        /* retrieve regex pattern */
	nbu_string_printf(buf, sizeof buf, "Env.Var.%s.Regex", var_name);

	pattern = hdf_get_value(cgi->hdf, buf, NULL);
	if (pattern == NULL)
	{
		return EWF_SUCCESS;
	}

	return ewf_utils_pcre_validate(value, pattern);
}

/*********************************************************************
 * SET'TER accessor
 ********************************************************************/
int ewf_hdf_setf(const char *var_name, const char *fmt, ...)
{
	va_list args;
	int ret = EWF_ERROR;
	
	va_start(args, fmt);
	ret = _ewf_hdf_vset(var_name, fmt, args, 0);
	va_end(args);
	
	return ret;
}

int ewf_hdf_set(const char *var_name, const char *str_value)
{
	return _ewf_hdf_set(var_name, str_value, 0);
}

int ewf_hdf_set_error(const char *var_name, const char *error_id)
{
	int ret = EWF_SUCCESS;
	char *val = NULL;
	
	if(!var_name)
	{
		nbu_log_error("Set error '%s' = '%d' FAILED (NULL component) !", 
			      var_name != NULL ? var_name : "NULL",
			      error_id);
		
		return EWF_ERROR;
	}
	
	if(nbu_string_printf(_internal_buf, sizeof _internal_buf, "Var.%s.Error", var_name) == NBU_ERROR)
	{
		nbu_log_error("Var '%s' can be incoherent or truncated !", _internal_buf);
		ret = EWF_ERROR;
	}
	
	if( (val = hdf_get_value(cgi->hdf, _internal_buf, NULL)) != NULL )
	{
		/* hdf var has already an value ! */
		nbu_log_debug("Error '%s' already exist, don't overwrite it !", _internal_buf);
		return EWF_ERROR;
	}
	
	hdf_set_value(cgi->hdf, _internal_buf, error_id);
	hdf_set_int_value(cgi->hdf, "Form.Status", -1);
	
	nbu_log_debug("Set error '%s' = '%s'", _internal_buf, error_id);

	return ret;
}

int ewf_hdf_form_set_error(const char *error_id)
{
	hdf_set_int_value(cgi->hdf, "Form.Status", -1);
	hdf_set_value(cgi->hdf, "Form.Error", error_id);
	
	nbu_log_debug("Set 'Form.Error' to '%s'", error_id);

	return EWF_SUCCESS;
}

int ewf_hdf_form_set_success(const char *success_id)
{
	hdf_set_int_value(cgi->hdf, "Form.Status", 0);
	hdf_set_value(cgi->hdf, "Form.Success", success_id);
	
	nbu_log_debug("Set 'Form.Success' to '%s'", success_id);

	return EWF_SUCCESS;
}

int ewf_hdf_owsetf(const char *var_name, const char *fmt, ...)
{
	va_list args;
	int ret = EWF_ERROR;
	
	va_start(args, fmt);
	ret = _ewf_hdf_vset(var_name, fmt, args, 1);
	va_end(args);
	
	return ret;
}

int ewf_hdf_owset(const char *var_name, const char *str_value)
{
	return _ewf_hdf_set(var_name, str_value, 1);
}

/*********************************************************************
 * GET'TER accessor
 ********************************************************************/
/*
 * 'var_name' is PREFIXED by 'Env.Var' and SUFFIXED by '.Value'
 */
const char* ewf_hdf_env_gettext(const char *var_name, const char *value)
{
	return ewf_hdf_env_gettext_d(var_name, value, NULL);
}

const char* ewf_hdf_env_gettext_d(const char *var_name, const char *value, const char *def_value)
{
	char *val = NULL;
	
	if(!var_name)
	{
		nbu_log_error("Get '%s' FAILED (NULL component) !", 
			      var_name != NULL ? var_name : "NULL");
		
		return "";
	}
	
	if(value == NULL || strlen(value) == 0)
	{
		/* value is empty */
		return "";
	}

	nbu_string_printf(_internal_buf, sizeof _internal_buf, 
			  "Env.Var.%s.Value.%s", var_name, value);
	val = hdf_get_value(cgi->hdf, _internal_buf, NULL);
	
	if(val == NULL)
	{
		nbu_log_error("'%s' hdf var doesn't exist !", _internal_buf);
		
		nbu_string_printf(_internal_buf, sizeof _internal_buf, 
				  "Env.Var.%s.Value.%s", var_name, def_value != NULL ? def_value : "NULL");
		val = hdf_get_value(cgi->hdf, _internal_buf, NULL);
		if(val == NULL)
		{
			nbu_log_error("'%s' hdf var doesn't exist !", _internal_buf);
			
			val = hdf_get_value(cgi->hdf, "Env.Text.UnknownValue", NULL);
			if(val == NULL)
			{
				nbu_log_error("'Env.Text.UnknownValue' hdf var doesn't exist !");
				val = "Unknown Value !";
			}
		}
	}
	
	nbu_log_debug("Env GetText \"%s\" = '%s'", var_name, val != NULL ? val : "(NULL)");
	
	return val;
}

const char* ewf_hdf_get_default(const char *var_name, const char *dfl_val)
{
	const char *val = NULL;
        char buf[128];

	nbu_string_printf(buf, sizeof buf, "Query.%s", var_name);

	val = hdf_get_value(cgi->hdf, buf, dfl_val);
        nbu_log_debug("Get '%s' = %s'%s'",
                      buf,
                      val == dfl_val ? "(default value) " : "",
                      val != NULL ? val : "(NULL)");

	return val;
}

const char* ewf_hdf_get(const char *var_name)
{
	return ewf_hdf_get_default(var_name, NULL);
}

int ewf_hdf_get_copy(const char *var_name, char *buf, size_t buf_len)
{
	const char *val = NULL;
	int ret = EWF_ERROR;

        val = ewf_hdf_get(var_name);
	if(val != NULL)
	{
		nbu_string_printf(buf, buf_len, "%s", val);
		ret = EWF_SUCCESS;
	}

	return ret;
}

int ewf_hdf_get_int_default(const char *var_name, int default_val_int)
{
        const char *val = NULL;
	int val_int;
	
        val = ewf_hdf_get(var_name);
	if(val == NULL)
	{
                return default_val_int;
        }

        errno = 0;
        val_int = strtol(val, NULL, 10);
        if(errno != 0)
        {
                val_int = default_val_int;
        }

        return val_int;
}

int ewf_hdf_get_int(const char *var_name)
{
	return ewf_hdf_get_int_default(var_name, -1);
}

const char* ewf_hdf_sget_default(const char *var_name, const char *dflval)
{
        const char *val = NULL;

	val = ewf_hdf_get_default(var_name, dflval);
	if(val == dflval)
        {
                return dflval;
        }

        if(ewf_hdf_validate(var_name, val) == EWF_ERROR)
	{
                return dflval;
        }

        return val;
}

const char* ewf_hdf_sget(const char *var_name)
{
        char buf[128];
	const char *val = NULL;

        val = ewf_hdf_get(var_name);
        if(val == NULL)
        {
                return NULL;
        }

        if(ewf_hdf_validate(var_name, val) == EWF_ERROR)
	{
                nbu_string_printf(buf, sizeof buf, "Var.%s.Error", var_name);
                hdf_set_value(cgi->hdf, buf, "value");
                hdf_set_int_value(cgi->hdf, "Form.Status", -1);

		val = NULL;
	}

	return val;
}

int ewf_hdf_sget_copy(const char *var_name, char *buf, size_t buf_len)
{
        const char *val = NULL;
        int ret = EWF_ERROR;

        val = ewf_hdf_sget(var_name);
        if(val != NULL)
        {
                nbu_string_copy(buf, buf_len, val);
                ret = EWF_SUCCESS;
        }

        return ret;
}

int ewf_hdf_sget_int_default(const char *var_name, int dflt_val_int)
{
        const char *val = NULL;
	int val_int;
	
	val = ewf_hdf_sget(var_name);
        if(val == NULL)
	{
                return dflt_val_int;
        }

        errno = 0;
        val_int = strtol(val, NULL, 10);
        if(errno != 0)
        {
                val_int = dflt_val_int;
        }

        return val_int;
}

int ewf_hdf_sget_int(const char *var_name)
{
        return ewf_hdf_sget_int_default(var_name, -1);
}

/*********************************************************************
 * Utils
 ********************************************************************/
void ewf_hdf_read_file(const char *name)
{
	NEOERR *err;
	
	err = hdf_read_file(cgi->hdf, name);
	if (err != NULL)
	{
		nerr_log_error(err);
		nerr_ignore(&err);
	}
}

struct ewf_hdf_iterator * ewf_hdf_each(const char *name, struct ewf_hdf_iterator *hdf_iterator)
{
        if(name != NULL)
        {       //first time
                hdf_iterator->node = hdf_iterator->parentNode = NULL;
                hdf_iterator->parentNode = hdf_get_obj(cgi->hdf, name);
                if(hdf_iterator->parentNode == NULL) {
                        nbu_log_debug("'%s' not exist !",name);
                        return NULL;
                }
                hdf_iterator->node = hdf_obj_child(hdf_iterator->parentNode);
        } 
        else
        {       //next time
                hdf_iterator->node = hdf_obj_next(hdf_iterator->node);
        }
        
        if(hdf_iterator->node != NULL)
        {
                hdf_iterator->name = hdf_obj_name(hdf_iterator->node);
                hdf_iterator->value = hdf_obj_value(hdf_iterator->node);
                nbu_log_debug("'%s.%s = %s'", hdf_obj_name(hdf_iterator->parentNode), hdf_iterator->name, hdf_iterator->value);
                return hdf_iterator;
        }
        return NULL;
}

int ewf_hdf_exist(const char *name)
{
	HDF* node = hdf_get_obj(cgi->hdf, name);
	short i = (node != NULL);
        
        nbu_log_debug("'%s' %s exist !", name, i ? "" : "not");
        
        return i;
}

int ewf_hdf_copy_cgi_query_tree(void)
{
        char val_buf[512];
        int ret;
        struct ewf_hdf_iterator query_hdf;

        EWF_HDF_FOREACH("Query", &query_hdf)
        {
                if(query_hdf.name != NULL && query_hdf.value != NULL)
                {
                        ret = ewf_secure_escape_input(query_hdf.value,
                                                      val_buf,
                                                      sizeof(val_buf));
                        if(ret != EWF_SUCCESS)
                        {
                                nbu_log_error("Error escaping input %s !",
                                              query_hdf.value);
                                continue;
                        }

                        /* set */
                        ewf_hdf_set(query_hdf.name, val_buf);
                }
        }

        return EWF_SUCCESS;
}

int ewf_hdf_set_env(const char *name, const char *value)
{
        char buf[128];
        
        nbu_string_printf(buf, sizeof(buf), "Env.%s", name);
         
        hdf_set_value(cgi->hdf, buf, value);

        return EWF_SUCCESS;
}

const char* ewf_hdf_get_env(const char *name)
{
        char buf[128];
        
        nbu_string_printf(buf, sizeof(buf), "Env.%s", name);

        return hdf_get_value(cgi->hdf, buf, NULL);
}

void ewf_hdf_cgi_parse(void)
{
	NEOERR *err = NULL;
        
        err = cgi_parse(cgi);
	nerr_ignore(&err);
}
