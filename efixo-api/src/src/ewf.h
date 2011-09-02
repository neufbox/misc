#ifndef _EWF_H_
#define _EWF_H_

#include "ClearSilver.h"
#include "fcgi_stdio.h"

#include "ewf_fastcgi.h"
#include "ewf_http.h"
#include "ewf_hashtable.h"

#include "nbu/nbu.h"
#include "nbu/string.h"
#include "nbu/list.h"
#include "nbu/net.h"
#include "nbu/log.h"

#define EWF_ERROR  -1
#define EWF_SUCCESS 0

#ifdef EWF_UNUSED
#elif defined(__GNUC__)
#define EWF_UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
#define EWF_UNUSED(x) /*@unused@*/ x
#else
#define EWF_UNUSED(x) x
#endif

/*! Global pointer to the CGI context.
 */
CGI *cgi;

#endif /* _EWF_H _ */

