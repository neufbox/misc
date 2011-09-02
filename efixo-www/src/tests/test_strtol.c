#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int n, i, v;
	char *endptr, *nptr;
	char *str[] = {"123", "12b", "b23", "1b3", "", NULL};

	n = 0;
	while (str[n] != NULL)
	{
		nptr = str[n];

		i = strtol(nptr, &endptr, 10);

		if ((nptr == endptr) || (*endptr != '\0'))
		{
			v = 0;
		}
		else
		{
			v = 1;
		}
		printf("%s -> %d - nptr: %x - endptr: %x - valid: %d\n", nptr, i, *nptr, *endptr, v);

		n++;
	}

	return 0;
}
