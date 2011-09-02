#ifndef _EWF_FCGI_H_
#define _EWF_FCGI_H_

#include "fcgiapp.h"

#include <string.h>

extern FCGX_Stream *fcgx_in, *fcgx_out, *fcgx_err;
extern FCGX_ParamArray fcgx_envp;

static inline int ewf_fcgi_printf(const char *format, ...)
{
	va_list args;
	int ret = -1;
	
	va_start(args, format);
	ret = FCGX_VFPrintF(fcgx_out, format, args);
	va_end(args);

	return ret;
}

static inline int ewf_fcgi_fwrite(const void *ptr, size_t size, size_t nmemb)
{
	int ret = -1;
	
	ret = (FCGX_PutStr((const char*)ptr, size * nmemb, fcgx_out) / size);

	return ret;
}

#endif
