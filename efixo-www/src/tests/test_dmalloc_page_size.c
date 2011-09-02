#include <stdio.h>

int main()
{
	int i;

	for (i = 11; i < 15; ++i)
	{
		if (getpagesize() <= 1 << i)
		{
			break;
		}
	}

	printf("%i\n", i);

	return 0;
}
