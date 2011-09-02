#include "ewf/ewf.h"

#include "ewf/core/ewf_secure.h"

#include <stdlib.h>
#include <string.h>
#include <pcre.h>

#include "nbu/nbu.h"
#include "nbu/log.h"

int ewf_secure_escape_input(const char *val, char *buf, size_t buf_size)
{
	static const int min_allowed_ascii_char = ' ',
		max_allowed_ascii_char = '~';
	static char *forbidden_ascii_chars = "\"'`";
	static char censure_char = ' ';
	
	unsigned int i_val = 0,
		i_forb = 0;
	int ret = EWF_ERROR;
	
	while(val[i_val] != '\0' 
	      && i_val < buf_size - 1)
	{
		if(val[i_val] < min_allowed_ascii_char
		   || val[i_val] > max_allowed_ascii_char)
		{
			/* bizarre ASCII character */
			buf[i_val] = censure_char;
		}
		else
		{
			buf[i_val] = val[i_val];

			i_forb = 0;
			
			while(forbidden_ascii_chars[i_forb] != '\0')
			{
				if(val[i_val] == forbidden_ascii_chars[i_forb])
				{
					buf[i_val] = censure_char;
					break;
				}
				
				i_forb++;
			}
		}

		i_val++;
	}

	if(val[i_val] == '\0')
	{
		ret = EWF_SUCCESS;
	}
	
	buf[i_val] = '\0';

	return ret;
}


