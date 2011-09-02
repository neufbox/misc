#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	char buffer[8];

	snprintf(buffer, 7, "%s", "1234567");
	printf("%s\n", buffer);

	snprintf(buffer, 8, "%s", "1234567");
	printf("%s\n", buffer);

	snprintf(buffer, 9, "%s", "1234567");
	printf("%s\n", buffer);

	snprintf(buffer, 7, "%s", "12345678");
	printf("%s\n", buffer);

	snprintf(buffer, 8, "%s", "12345678");
	printf("%s\n", buffer);

	snprintf(buffer, 9, "%s", "12345678");
	printf("%s\n", buffer);

	snprintf(buffer, 7, "%s", "123456789");
	printf("%s\n", buffer);

	snprintf(buffer, 8, "%s", "123456789");
	printf("%s\n", buffer);

	snprintf(buffer, 9, "%s", "123456789");
	printf("%s\n", buffer);

	return 0;
}
