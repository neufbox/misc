#ifndef _EWF_FASTCGI_H_
#define _EWF_FASTCGI_H_

/* FCGI_fread wrapper for ClearSilver CGI */
int ewf_fastcgi_read_cb(void *ptr, char *data, int size);

/* FCGI_vprintf wrapper for ClearSilver CGI */
int ewf_fastcgi_writef_cb(void *ptr, const char *format, va_list ap);

/* FCGI_fwrite wrapper for ClearSilver CGI */
int ewf_fastcgi_write_cb(void *ptr, const char *data, int size);

void ewf_fastcgi_set_content_type(const char *type);

void ewf_fastcgi_parse(void);

int ewf_fastcgi_accept(void);

/*! Display.
 */
void ewf_fastcgi_display(const char *name);

int ewf_fastcgi_get_parameter(const char *name, char **value);

#endif /* _EWF_FASTCGI_H_ */

