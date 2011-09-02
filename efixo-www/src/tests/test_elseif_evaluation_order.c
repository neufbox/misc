#include <stdio.h>

int main(int argc, char **argv)
{
	char buf[8];

	strcpy(buf, "hi");

	if (strcmp(buf, "hi") == 0)
	{
		printf("hi\n");
		strcpy(buf, "bye");
	}
	else if (strcmp(buf, "bye") == 0)
	{
		printf("bye\n");
	}

	return 0;
}
