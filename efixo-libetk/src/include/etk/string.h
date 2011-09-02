/*!
 * \file etk/string.h
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

#ifndef _ETK_STRING_H_
#define _ETK_STRING_H_ 1

#include <string.h>

/*! \def hex_asc
 *  \brief Convert hex to ascii
 */
#define hex_ascii(x)	"0123456789abcdef"[x]

/*! \def matches(s, c_str)
 *  \brief Compare strings.
 *  \param data Data strings
 *  \param cstr C-Strings
 */
#undef matches
#define matches(s, c_str) \
({ \
	const char __dummy[] = c_str; \
	(void)(&__dummy); \
	(memcmp (s, c_str, sizeof(c_str)) == 0); \
})

/*! \def for_each_line(t,s)
 */
#define for_each_line(t,s) \
	for( char *ctx, *t = strtok_r( s, "\n\r", &ctx ); t; \
			t = strtok_r( NULL, "\n\r", &ctx ) )

/*! \def for_each_token(t,s)
 */
#define for_each_token(t,s) \
	for( char *ctx, *t = strtok_r( s, " \t\n\r", &ctx ); t; \
	     t = strtok_r( NULL, " \t\n\r", &ctx ) )

/* strrtrim()
 */
char const *strrtrim(char *);

/* strltrim()
 */
char const *strltrim(char const *);

/* strtrim()
 */
static inline char const *strtrim(char *str)
{
	return strltrim(strrtrim(str));
}

/*! safe_strtol()
 */
long safe_strtol(char const *, long);

/*! safe_strtoul()
 */
long safe_strtoul(char const *, long);

size_t strlcpy(char *, const char *, size_t);

size_t strlcat(char *, const char *, size_t);

void pack(char const *, unsigned char *, size_t);


#endif
