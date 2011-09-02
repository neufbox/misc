#include <stdio.h>

int main(int argc, char **argv)
{
	printf("%s %s %s\n", strndup("123456", 5), strndup("123456", 6), strndup("123456", 7));

	return 0;
}
