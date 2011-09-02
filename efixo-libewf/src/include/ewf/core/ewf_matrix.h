#ifndef _EWF_MATRIX_H_
#define _EWF_MATRIX_H_

#include "ewf/ewf_page.h"

#include "ewf/core/ewf_session.h"
#include "ewf/core/ewf_request.h"

#include "nbu/list.h"

#include <string.h>

typedef struct ewf_matrix_vars_t {
	char *version;
} ewf_matrix_vars_t;

extern ewf_matrix_vars_t ewf_matrix_vars;

typedef struct ewf_matrix_ops_t {
	int (*core_ext_init)(void);
	void (*core_ext_cleanup)(void);
	void (*pre_dispatch)(ewf_request_t *request);
	void (*pre_render)(ewf_request_t *request, ewf_page_t *page);
	int (*conf_get)(const char* name, char *val, size_t val_size);
	int (*conf_set)(const char* name, const char *val);
	int (*login_alt_method)(ewf_session_t *session,
				const char *method);
 	int (*preload_theme_alt_scheme)(const char *scheme,
					const char *hlpart,
					char *lthdir,
					size_t lthdir_size);
	int (*getlist_repo_theme_alt_scheme)(nbu_list_t *th_repo_list);
} ewf_matrix_ops_t;

extern ewf_matrix_ops_t ewf_matrix_ops;

#endif
