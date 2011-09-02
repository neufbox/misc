#include "nbu/file.h"

#include <string.h>

#include "nbu/nbu.h"

const char *nbu_file_basename(const char *path)
{
	const char *c = NULL;

	if ((path == NULL) || (*path == '\0'))
	{
		return NULL;
	}

	if ((*path == '/') && (*(path + 1) != '\0'))
	{
		c = path + strlen(path) - 1;

		while ((c > path) && (*c != '/'))
		{
			c--;
		}

		c++;
	}
	else
	{
		c = path;
	}

	return c;
}
