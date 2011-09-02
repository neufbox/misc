#ifndef _EWF_UTILS_H_
#define _EWF_UTILS_H_

#include <stdio.h>

/*
 * get client ip
 */
extern const char *ewf_utils_client_ip(void);

/*
 * get client referer
 */
extern const char *ewf_utils_client_referer(void);

/*
 * redirect client to another page
 */
extern void ewf_utils_redirect(const char *uri);

/*
 * get file handle
 */
extern FILE *ewf_utils_filehandle(const char *name);

/*
 * check client referer. 
 * @return EWF_SUCCESS if page name submitted is where client come from
 */
extern int ewf_utils_check_referer(const char *page);

/*! Validate 'value' against 'pattern'.
 * \return
 * - \b EWF_SUCCESS if the value matches the pattern
 * - \b EWF_ERROR otherwise, or if 'value' or 'pattern' is NULL
 */
extern int ewf_utils_pcre_validate(const char *value, const char *pattern);

#endif
