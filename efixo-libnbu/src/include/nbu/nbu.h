#ifndef _NBU_H_
#define _NBU_H_

/* Please remove this things */
#include "nbu/file.h"
#include "nbu/list.h"
#include "nbu/string.h"
#include "nbu/util.h"
#include "nbu/log.h"

#define NBU_ERROR  -1
#define NBU_SUCCESS 0

#define NBU_UNUSED(x) ( (void)(x) )

#ifdef DEBUG
#  define nbu_free(ptr) do { \
	if(ptr == NULL)	     \
	{ \
		nbu_log_debug("pointer '%s' is NULL !", #ptr);	\
	} \
	else \
	{ \
		free((ptr));	      \
		(ptr) = NULL;	      \
	} \
	} while(0)
#else
#  define nbu_free(ptr) do { \
			free((ptr)); \
			(ptr) = NULL; \
		} while(0)
#endif

#define  NBU_ARRAY_SIZE( x ) (sizeof ( x ) / sizeof ( ( x ) [0] ) )

#endif /* _NBU_H_ */
