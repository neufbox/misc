#ifndef EWF_THEME_H
#define EWF_THEME_H

#include <nbu/list.h>

enum ewf_theme_load_return {
	EWF_THEME_LOAD_ERROR = -1000,
	EWF_THEME_LOAD_INVALID_URI,
	EWF_THEME_LOAD_UNKNOWN_SCHEME,
	EWF_THEME_LOAD_INSTALL_FAILED,
	EWF_THEME_LOAD_SUCCESS = 0,
};

enum ewf_theme_parse_return {
        EWF_THEME_PARSE_ERROR = -900,
        EWF_THEME_PARSE_NOXML,
        EWF_THEME_PARSE_INVALIDXML,
        EWF_THEME_PARSE_MISSVALUE,
        EWF_THEME_PARSE_INVALID_UUID,
        EWF_THEME_PARSE_INVALID_VERSION,
        EWF_THEME_PARSE_INVALID_TARGET,
        EWF_THEME_PARSE_SUCCESS,
};

typedef struct ewf_theme_t {
        char uuid[37];
	char name[16];
	char longname[32];
	char version[16];
	char target[32];
	char author[64];
	char description[128];
	char homepage[128];
} ewf_theme_t;

typedef struct ewf_theme_ipkg_t {
	char uri[256];
        short unsigned int has_preview;
        short unsigned int is_compatible;
	ewf_theme_t theme;
} ewf_theme_ipkg_t;

typedef struct ewf_theme_repo_t {
	char uri[256];
	char pwd[256];
} ewf_theme_repo_t;

extern int ewf_theme_load(void);
extern int ewf_theme_get_list(nbu_list_t **th_ipkg_list);
extern int ewf_theme_set(const char *uri);
extern int ewf_theme_cut_uri(const char *uri,
			     char *scheme,
			     size_t scheme_size,
			     char *hlpart,
			     size_t htpart_size);
extern const char* ewf_theme_uri_basename(const char *uri);

#endif
