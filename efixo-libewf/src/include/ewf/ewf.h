#ifndef _EWF_H_
#define _EWF_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <nbu/nbu.h>
#include <nbu/string.h>
#include <nbu/net.h>
#include <nbu/list.h>

#define EWF_ERROR  -1
#define EWF_SUCCESS 0

#define APP_ERROR_112 112
#define APP_ERROR_115 115
#define HTTP_ERROR_404 404

#define URI_PAGE_INDEX "/index"
#define URI_PAGE_LOGIN "/login"
#define URI_PAGE_LOGOUT "/logout"
#define URI_PAGE_ERROR "/error"

#endif
