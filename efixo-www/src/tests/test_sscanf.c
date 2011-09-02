#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int i;

	if (sscanf("255", "%3d", &i) == 1)
	{
		printf("255 -> %d\n", i);
	}
	else
	{
		printf("255 -> error\n");
	}

	if (sscanf("1234", "%3d", &i) == 1)
	{
		printf("1234 -> %d\n", i);
	}
	else
	{
		printf("1234 -> error\n");
	}

	if (sscanf("9", "%3d", &i) == 1)
	{
		printf("9 -> %d\n", i);
	}
	else
	{
		printf("9 -> error\n");
	}

	if (sscanf("9d", "%3d", &i) == 1)
	{
		printf("9d -> %d\n", i);
	}
	else
	{
		printf("9d -> error\n");
	}

	if (sscanf("d9", "%3d", &i) == 1)
	{
		printf("d9 -> %d\n", i);
	}
	else
	{
		printf("d9 -> error\n");
	}

	return 0;
}
