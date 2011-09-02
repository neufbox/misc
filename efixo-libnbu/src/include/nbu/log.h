#ifndef _NBU_LOG_H_
#define _NBU_LOG_H_

#include <syslog.h>

#include "nbu/file.h"

/********* colors - vt102 - ********/
#define COLOR_ESCAPE		"\033"

#define COLOR_RESET		COLOR_ESCAPE "[0m"

#define COLOR_BLACK(txt)	COLOR_ESCAPE "[0;30m" txt COLOR_RESET
#define COLOR_RED(txt)		COLOR_ESCAPE "[0;31m" txt COLOR_RESET
#define COLOR_GREEN(txt)	COLOR_ESCAPE "[0;32m" txt COLOR_RESET
#define COLOR_BROWN(txt)	COLOR_ESCAPE "[0;33m" txt COLOR_RESET
#define COLOR_BLUE(txt)		COLOR_ESCAPE "[0;34m" txt COLOR_RESET
#define COLOR_PURPLE(txt)	COLOR_ESCAPE "[0;35m" txt COLOR_RESET
#define COLOR_CYAN(txt)		COLOR_ESCAPE "[0;36m" txt COLOR_RESET
#define COLOR_GRAY(txt)		COLOR_ESCAPE "[0;37m" txt COLOR_RESET

#define COLOR_DARK_GRAY(txt)	COLOR_ESCAPE "[1;30m" txt COLOR_RESET
#define COLOR_LIGHT_RED(txt)	COLOR_ESCAPE "[1;31m" txt COLOR_RESET
#define COLOR_LIGHT_GREEN(txt)	COLOR_ESCAPE "[1;32m" txt COLOR_RESET
#define COLOR_YELLOW(txt)	COLOR_ESCAPE "[1;33m" txt COLOR_RESET
#define COLOR_LIGHT_BLUE(txt)	COLOR_ESCAPE "[1;34m" txt COLOR_RESET
#define COLOR_LIGHT_PURPLE(txt)	COLOR_ESCAPE "[1;35m" txt COLOR_RESET
#define COLOR_LIGHT_CYAN(txt)	COLOR_ESCAPE "[1;36m" txt COLOR_RESET
#define COLOR_WHITE(txt)	COLOR_ESCAPE "[1;37m" txt COLOR_RESET
/**********************************/


#define nbu_log_info(fmt, ...)                                          \
	nbu_log_write(LOG_INFO, fmt, ##__VA_ARGS__)

#define nbu_log_notice(fmt, ...)                                        \
        nbu_log_write(LOG_NOTICE, "in %s:%04d:%s - " COLOR_PURPLE(fmt), \
                      nbu_file_basename(__FILE__), __LINE__,            \
                      __FUNCTION__, ##__VA_ARGS__)
        
#define nbu_log_error(fmt, ...)                                         \
        nbu_log_write(LOG_ERR, "in %s:%04d:%s - " COLOR_RED(fmt),       \
                      nbu_file_basename(__FILE__), __LINE__,            \
                      __FUNCTION__, ##__VA_ARGS__)

#ifdef DEBUG
#define nbu_log_debug(fmt, ...)                                         \
        nbu_log_write(LOG_DEBUG, "in %s:%04d:%s - " fmt,                \
                      nbu_file_basename(__FILE__), __LINE__,            \
                      __FUNCTION__, ##__VA_ARGS__)
#else
#define nbu_log_debug(fmt, ...) do {;} while(0)
#endif

void nbu_log_open(const char *ident);
void nbu_log_close(void);
void nbu_log_write(int level, const char *fmt, ...);

#endif /* _NBU_LOG_H_ */
