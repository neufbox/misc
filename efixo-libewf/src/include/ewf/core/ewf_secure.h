#ifndef _EWF_SECURE_H_
#define _EWF_SECURE_H_

extern const char* ewf_secure_xss_escape(const char *buf);

extern int ewf_secure_escape_input(const char *val, char *buf, size_t buf_size);

#endif
