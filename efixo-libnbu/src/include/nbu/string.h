#ifndef _NBU_STRING_H_
#define _NBU_STRING_H_

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "nbu/list.h"

#define NBU_STRING_EQUAL 0
#define NBU_STRING_DIFFERENT -1

/*! Check whether a string is NULL or empty.
 * \param str string
 * \return
 * - \b NBU_SUCCESS if the string is empty
 * - \b NBU_ERROR if not
 */
int nbu_string_isempty(const char *str);

/*! Wrapper for strcmp.
 * \param str1 string
 * \param str2 string
 * \return
 * - \b NBU_STRING_EQUAL if the strings are equal
 * - \b NBU_STRING_DIFFERENT if the strings are different
 */
int nbu_string_compare(const char *str1, const char *str2);

/*! Wrapper for strncmp.
 * \param str1 string
 * \param maxsize maximum size of the string,
 *        ie size of the buffer containing it (sizeof buffer),
 *        or maximum length of the string + 1
 * \param str2 string
 * \return
 * - \b NBU_STRING_EQUAL if the strings are equal
 * - \b NBU_STRING_DIFFERENT if the strings are different
 */
int nbu_string_ncompare(const char *str1, size_t maxsize, const char *str2);

/*! strcmp with static string in second argument.
 * \param data Data string
 * \param cstr Static string (need to be static !)
 * \return
 * - \b NBU_STRING_EQUAL if the strings are equal
 * - \b NBU_STRING_DIFFERENT if the strings are different
 */
#define nbu_string_matches( s, c_str )					\
	({								\
		const char __dummy[] = c_str;				\
		(void)(&__dummy);					\
		(memcmp ( s, c_str, sizeof( c_str ) ) == 0 ? NBU_STRING_EQUAL : NBU_STRING_DIFFERENT); \
	})

/*! Wrapper for strlen.
 * \param str string
 * \param maxsize maximum size of the string,
 *        ie size of the buffer containing it (sizeof buffer),
 *        or maximum length of the string + 1
 * \return length of the string
 */
size_t nbu_string_length(const char *str, size_t maxlen);

/*! Wrapper for strcpy.
 * \param str destination buffer
 * \param size size of the buffer (sizeof buffer)
 * \param src string to be copied
 * \return
 * - \b NBU_SUCCESS on success
 * - \b NBU_ERROR on error or truncation
 */
int nbu_string_copy(char *str, size_t size, const char *src);

/*! Wrapper for vsnprintf.
 * \param str destination buffer
 * \param size size of the buffer (sizeof buffer)
 * \param fmt format string
 * \param args arguments
 * \return
 * - \b NBU_SUCCESS on success
 * - \b NBU_ERROR on error or truncation
 */
int nbu_string_vprintf(char *str, size_t size, const char *fmt, va_list args);

/*! Wrapper for snprintf.
 * \param str destination buffer
 * \param size size of the buffer (sizeof buffer)
 * \param fmt format string
 * \param ... arguments
 * \return
 * - \b NBU_SUCCESS on success
 * - \b NBU_ERROR on error or truncation
 */
int nbu_string_printf(char *str, size_t size, const char *fmt, ...);

/*!
 * split string with an separator caracter. Think to destroy list after.
 * \param str string to split
 * \param sep separator string
 * \param list result in nbu_list_t type list
 * \return
 * - \b NBU_SUCCESS on success
 * - \b NBU_ERROR on error
 */
int nbu_string_split(const char *str, const char *sep, nbu_list_t **list);

/*!
 * trim string (remove space, \t, \n, \r at start and end of the string).
 * \param str string to time
 * \param buf result buf
 * \param buf_size size of buf
 * \return
 * - \b NBU_SUCCESS on success
 * - \b NBU_ERROR on error
 */
int nbu_string_trim(const char *str, char *buf, size_t buf_size);

/*!
 * return int value contained in the string (like strtol but better)
 * \param str string contain the int
 * \return the int value if success, otherwise return default_value
 */
int nbu_string_to_int(const char *str, int default_value);

#endif /* _NBU_STRING_H_ */
