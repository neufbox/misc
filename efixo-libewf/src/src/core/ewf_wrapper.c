#include "ewf/ewf.h"

#include "ewf/core/ewf_wrapper.h"
#include "ewf/core/ewf_fcgi.h"

#include "ClearSilver.h"

#include "nbu/nbu.h"

int ewf_wrapper_read_cb(void *ptr, char *data, int size)
{
	size_t n;
	
	NBU_UNUSED(ptr);
	
/* 	nbu_log_debug("+ read_cb"); */
	
	n = FCGX_GetStr(data, sizeof(char) * size, fcgx_in);
	
	return (n / sizeof(char));
}

int ewf_wrapper_writef_cb(void *ptr, const char *format, va_list ap)
{
	NBU_UNUSED(ptr);
	
/* 	nbu_log_debug("+ writef_cb"); */
	
	FCGX_VFPrintF(fcgx_out, format, ap);
	
	return 0;
}

int ewf_wrapper_write_cb(void *ptr, const char *data, int size)
{
	size_t n;
	
	NBU_UNUSED(ptr);
	
/* 	nbu_log_debug("+ write_cb, size = %d", size); */
	
	n = FCGX_PutStr(data, sizeof(char) * size, fcgx_out);
	
	return (n / sizeof(char));
}

char *ewf_wrapper_getenv_cb(void *ptr, const char *name)
{
	char *value = NULL;
	
	NBU_UNUSED(ptr);
	
/*  	nbu_log_debug("+ getenv_cb, name = %s, value = %s", name, FCGX_GetParam(name, fcgx_envp)); */
	
	value = FCGX_GetParam(name, fcgx_envp);
	return (value ? strdup(value) : NULL);
}

int ewf_wrapper_putenv_cb(void *ptr, const char *name, const char *value)
{
	NBU_UNUSED(ptr);
	NBU_UNUSED(name);
	NBU_UNUSED(value);
	
/* 	nbu_log_debug("+ putenv, name = %s, value = %s", name, value); */
	
	return 1;
}

int ewf_wrapper_iterenv_cb(void *ptr, int inx, char **name, char **value)
{
	char *eq = NULL;
	
	NBU_UNUSED(ptr);
	
	nbu_log_debug("+ iterenv !");

	*name = NULL;
	*value = NULL;

	/* trust that caller will start to index 0 */
	if(fcgx_envp[inx] != NULL)
	{
		eq = strchr(fcgx_envp[inx], '=');
		if(eq)
		{
			*name = strndup(fcgx_envp[inx], eq - fcgx_envp[inx]);
			*value = strdup(eq + 1);
		}
		if(*name == NULL || *value == NULL)
		{
			free(*name);
			free(*value);
			return 1;
		}
	}
	
	return 0;
}
