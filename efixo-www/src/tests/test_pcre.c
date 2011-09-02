#include <stdio.h>
#include "pcre.h"

#ifdef DMALLOC
#define FINI_DMALLOC 0
#include "dmalloc.h"
#endif

/* mips-linux-gcc -DDMALLOC -DDMALLOC_FUNC_CHECK -I/home/rhk/workspace/dev/staging/include test_pcre.c -L/home/rhk/workspace/dev/staging/lib -Wl,-Bdynamic -lpcre -Wl,-Bstatic -ldmalloc -o test_pcre */

int main()
{
	const char *pattern = "^[a-zA-Z0-9]{1,20}$";
	const char *value = "abcd123";
	const char *errorptr;
	int erroroffset;
	int rc = -1;
	pcre *re;

	re = pcre_compile(pattern, 0, &errorptr, &erroroffset, NULL);
	if (re != NULL)
	{
		rc = pcre_exec(re, NULL, value, strlen(value), 0, 0, NULL, 0);
		pcre_free(re);

		if (rc == 0)
		{
			printf("pcre_exec: '%s' matches '%s'\n", value, pattern);
		}
		else
		{
			printf("pcre_exec: '%s' does not match '%s'\n", value, pattern);
		}
	}
	else
	{
		printf("pcre_compile: error (%s)\n", errorptr);
	}

	return 0;
}
