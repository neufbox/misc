#ifndef _EWF_CONFIG_H_
#define _EWF_CONFIG_H_

#include <string.h>

typedef struct ewf_config_env_t {
	char *dir_vdocroot;    /* virtual docroot */
	char *dir_webapproot;  /* web app directory */
} ewf_config_env_t;

extern ewf_config_env_t ewf_config_env;

extern void ewf_config_initenv(void);

extern int ewf_config_set(const char *name, 
			  const char *value);

extern int ewf_config_get(const char *name, 
			  char *value, 
			  size_t value_size);

extern int ewf_config_get_int(const char *name);

#endif
