#ifndef _EWF_WRAPPER_H_
#define _EWF_WRAPPER_H_

#include <stdarg.h>

/* FCGI_fread wrapper for clearsilver CGI */
int ewf_wrapper_read_cb(void *ptr, char *data, int size);

/* FCGI_vprintf wrapper for clearsilver CGI */
int ewf_wrapper_writef_cb(void *ptr, const char *format, va_list ap);

/* FCGI_fwrite wrapper for clearsilver CGI */
int ewf_wrapper_write_cb(void *ptr, const char *data, int size);

/* getenv wrapper to use fastcgi with clearsilver */
char *ewf_wrapper_getenv_cb(void *ptr, const char *name);

/* putenv wrapper to use fastcgi with clearsilver */
int ewf_wrapper_putenv_cb(void *ptr, const char *name, const char *value);

/* iterenv wrapper to use fastcgi with clearsilver */
int ewf_wrapper_iterenv_cb(void *ptr, int inx, char **name, char **value);

#endif
