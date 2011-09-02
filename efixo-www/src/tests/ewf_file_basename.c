#include "ewf.h"

int main(int argc, char **argv)
{
	char *bn;
	char *list[] = {"/usr/lib", "/usr", "usr", "/", ".", "..", "", NULL};
	int n;

	n = 0;
	while (list[n] != NULL)
	{
		bn = ewf_basename(list[n]);

		if (bn != NULL)
		{
			printf("path: %s -> basename: %s\n", list[n], bn);
		}
		else
		{
			printf("path: %s -> error\n", list[n]);
		}

		n++;
	}

	return 0;
}
