#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	char buffer[16];
	char *s, *t;

	s = "123456";

	strncpy(buffer, s, 5);
	printf("%s\n", buffer);

	strncpy(buffer, s, 6);
	printf("%s\n", buffer);

	strncpy(buffer, s, 7);
	printf("%s\n", buffer);

	t = "0123456789abcdefghij";

	strncpy(buffer, t, sizeof buffer);
	printf("%s\n", buffer);

	strncpy(buffer, t, sizeof buffer - 1);
	printf("%s\n", buffer);

	return 0;
}
