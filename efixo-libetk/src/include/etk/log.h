/*!
 * \file etk/log.h
 *
 * \author Copyright 2010 Miguel GAIO <miguel.gaio@efixo.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _ETK_LOG_H_
#define _ETK_LOG_H_ 1

#include <syslog.h>

#include <etk/vt102.h>


/*! \def fatal(fmt, ...)
 *  \brief Log message. Set led alarm slow blinks. Exit server.
 *  \param fmt Log message
 *  \param ... Arguments list
 */
#define fatal(fmt, ...) \
do { \
    syslog(LOG_CRIT, __LIGHT_RED("%s %s "fmt" %m"), \
                 "--- FATAL ---", __func__, ##__VA_ARGS__); \
} while (0)

/*! \def err(fmt, ...)
 *  \brief Log message.
 *  \param fmt Log message
 *  \param ... Arguments list
 */
#define err(fmt, ...) \
do { \
    syslog(LOG_ERR, __RED("%s::%s "fmt), \
    		 "error", __func__, ##__VA_ARGS__); \
} while (0)

/*! \def sys_err(fmt, ...)
 *  \brief Log message.
 *  \param fmt Log message
 *  \param ... Arguments list
 */
#define sys_err(fmt, ...) \
do { \
    syslog(LOG_ERR, __RED("%s::%s "fmt" %m"), \
    		 "error", __func__, ##__VA_ARGS__); \
} while (0)

/*! \def warn(fmt, ...)
 *  \brief Log message.
 *  \param fmt Log message
 *  \param ... Arguments list
 */
#define warn(fmt, ...) \
do { \
    syslog(LOG_WARNING, __PURPLE("%s::%s "fmt), \
    		 "warning", __func__, ##__VA_ARGS__); \
} while (0)

/*! \def note(...)
 *  \brief Log message.
 *  \param ... Log message and arguments list
 */
#define note(fmt, ...) \
do { \
    syslog(LOG_NOTICE, fmt, ##__VA_ARGS__); \
} while (0)

/*! \def info(...)
 *  \brief Log message. Enable in info or higher level
 *  \param ... Log message and arguments list
 */
#define info(fmt, ...) \
do { \
	extern int syslog_level; \
 \
	if (LOG_INFO <= syslog_level) \
		syslog(LOG_INFO, fmt, ##__VA_ARGS__); \
} while (0)

/*! \def dbg(fmt, ...)
 *  \brief Log message. Enable in debug level
 *  \param fmt Log message
 *  \param ... Arguments list
 */
#define dbg(fmt, ...) \
do { \
	extern int syslog_level; \
 \
	if (LOG_DEBUG <= syslog_level) \
		syslog(LOG_DEBUG, __GREY("%s %s "fmt), \
                	 "--- DEBUG ---", __func__, ##__VA_ARGS__); \
} while (0)

#ifdef DEBUG
/*! \def trace(fmt, ...)
 *  \brief Log message. Enable in devel level
 *  \param fmt Log message
 *  \param ... Arguments list
 */
#define trace(fmt, ...) \
do { \
	extern int syslog_level; \
 \
	if (LOG_DEBUG <= syslog_level) \
		syslog(LOG_DEBUG, __GREY("%s %s(" fmt ")"), \
                	"--- DEVEL ---", __func__, ##__VA_ARGS__); \
} while (0)
#else
#define trace(fmt, ...)
#endif

extern int syslog_level;

void setup_log_trigger(void);

#endif
