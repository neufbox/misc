#include "nbu/string.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "nbu/nbu.h"


int nbu_string_isempty(const char *str)
{
	if (str == NULL)
	{
		return NBU_SUCCESS;
	}
	
	if (str[0] == '\0')
	{
		return NBU_SUCCESS;
	}

	return NBU_ERROR;
}

int nbu_string_compare(const char *str1, const char *str2)
{
	const char *c1, *c2;

	if ((str1 == NULL) && (str2 == NULL))
	{
		return NBU_STRING_DIFFERENT;
	}

	if ((str1 == NULL) || (str2 == NULL))
	{
		return NBU_STRING_DIFFERENT;
	}

	c1 = str1;
	c2 = str2;

	while ((*c1 == *c2) && (*c1 != '\0') && (*c2 != '\0'))
	{
		c1++;
		c2++;
	}

	return (*c1 == *c2 ? NBU_STRING_EQUAL : NBU_STRING_DIFFERENT);
}

/* https://buildsecurityin.us-cert.gov/daisy/bsi-rules/home/g1/847.html
 */
int nbu_string_ncompare(const char *str1, size_t maxsize, const char *str2)
{
	if ((int) maxsize < 0)
	{
		return NBU_STRING_DIFFERENT;
	}
	
	if (strncmp(str1, str2, maxsize) != 0)
	{
		return NBU_STRING_DIFFERENT;
	}
	
	return NBU_STRING_EQUAL;
}

/* https://buildsecurityin.us-cert.gov/daisy/bsi-rules/home/g1/852.html
 */
size_t nbu_string_length(const char *str, size_t maxsize)
{
	if (maxsize <= 0)
	{
		return 0;
	}
	
	return strnlen(str, (int) (maxsize) - 1);
}

/* https://buildsecurityin.us-cert.gov/daisy/bsi-rules/home/g1/854.html
 * 1. passing of NULL for src or dest causes exception
 * 2. 'count' size parameter is often incorrectly passed in
 * 3. not guaranteed to have null terminated string upon exit
 */
int nbu_string_copy(char *str, size_t size, const char *src)
{
	int len = 0;
	
	if ((src == NULL)
	 || (str == NULL)
	 || ((int) size < 0)
	 || ((sizeof str[0]) == 0))
	{
		return NBU_ERROR;
	}
	
	len = (int) (size / (sizeof str[0]));
	
	/* NULL terminate the destination */
	str[len - 1] = '\0';
	strncpy(str, src, len);
	
	/* check that the destination is still NULL terminated */
	if (str[len - 1] != '\0')
	{
		/* the copied string has been truncated */
		str[len - 1] = '\0';
		return NBU_ERROR;
	}
	
	return NBU_SUCCESS;
}

int nbu_string_vprintf(char *str, size_t size, const char *fmt, va_list args)
{
	int ret = NBU_ERROR;
	
	if ((str == NULL) || (fmt == NULL)
	 || ((int) size < 0))
	{
		return NBU_ERROR;
	}

	/* put a NULL character at the last byte of the buffer
	 * to check for truncation after snprintf
	 */
	str[size - 1] = '\0';

	ret = vsnprintf(str, size, fmt, args);
	
	/* check if an error occurred */
	if (ret < 0)
	{
		return NBU_ERROR;
	}

	/* check if a truncation occurred */
	if ((str[size - 1] != '\0')
	 || (ret > (int) (size) - 1))
	{
		/* buffer has been truncated */
		str[size - 1] = '\0';
		return NBU_ERROR;
	}
	
	return NBU_SUCCESS;
}

/* https://buildsecurityin.us-cert.gov/daisy/bsi-rules/home/g1/838.html
 * In general, snprintf
 * 1. can return a negative number if the buffer is too small
 * 2. can return the number of bytes that it should have written
 * 3. might not null terminate the string
 * 4. might null terminate the string
 */
int nbu_string_printf(char *str, size_t size, const char *fmt, ...)
{
	va_list args;
	int ret = NBU_ERROR;
	
	if ((str == NULL)
	 || ((int) size < 0))
	{
		return NBU_ERROR;
	}

	va_start(args, fmt);
	ret = nbu_string_vprintf(str, size, fmt, args);
	va_end(args);
	
	return ret;
}

int nbu_string_split(const char *str, const char *sep, nbu_list_t **list)
{
	const char *saveptr = NULL;	/* pointer on str */
	char *item = NULL;   /* pointer of the current item */
	int len_sep = strlen(sep);
	
	*list = nbu_list_new(sizeof(char *));
	
	saveptr = str;
	while((item = strstr(saveptr, sep)) != NULL)
	{
		nbu_list_insert_end(*list, strndup(saveptr, item - saveptr));
		saveptr = item + len_sep;
	}

	/* copy end of string */
	nbu_list_insert_end(*list, strdup(saveptr));

	return NBU_SUCCESS;
}

int nbu_string_trim(const char *str, char *buf, size_t buf_size)
{
	int ret = NBU_ERROR;
	const char *p_str_sta = NULL, *p_str_end = NULL;
	int diff = 0;

	if(str == NULL)
	{
		return ret;
	}

	p_str_sta = str;
	p_str_end = str + (strlen(str) - 1);
	

#define TRIM(p_str, inc) \
	while(p_str != NULL) \
	{\
		if(p_str[0] != ' '\
		   && p_str[0] != '\t'\
		   && p_str[0] != '\n'\
		   && p_str[0] != '\r')\
		{\
			break;\
		}\
		p_str inc;\
	}

	TRIM(p_str_sta, ++);
	TRIM(p_str_end, --);
	
#undef TRIM
	
	diff = p_str_end - p_str_sta + 1;
	if(diff < 0)
	{
		diff = 0;
	}
	
	return nbu_string_printf(buf, buf_size, "%.*s", diff, p_str_sta);
}

int nbu_string_to_int(const char *str, int default_value)
{
	int res;
	
	errno = 0;
	
	res = strtol(str, NULL, 10);
	if(errno != 0)
	{
		res = default_value;
	}

	return res;
}
