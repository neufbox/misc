#include <stdio.h>
#include "common.h"

int main(void)
{
	char *list[] = {"toto.ath.cx", "t.o.t.o.ath.cx", ".ath.cx", "toto.", ".", "", NULL};
	char host[256];
	char domain[256];
	int n;

	n = 0;
	while (list[n] != NULL)
	{
		if (split_hostname(list[n], host, domain) == 0)
		{
			printf("split_hostname: %s -> %s / %s\n", list[n], host, domain);
		}
		else
		{
			printf("split_hostname: %s -> error\n", list[n]);
		}

		n++;
	}

	return 0;
}
