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

		if (*endptr == '\0')
		{
			v = 1;
		}
		else
		{
			v = 0;
		}
		printf("%s %d - nptr: %c - endptr: %c - valid: %d\n", nptr, i, *nptr, *endptr, v);

		n++;
	}

	return 0;
}
