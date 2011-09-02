#include <stdio.h>
#include <string.h>
#include "ewf.h"

int main(int argc, char **argv)
{
	char buffer[8];
	int ret = -1;

	ret = ewf_string_printf(buffer, sizeof buffer, "%s", "1234567");
	printf("%s (ret = %d)\n", buffer, ret);

	ret = ewf_string_printf(buffer, sizeof buffer, "%s", "12345678");
	printf("%s (ret = %d)\n", buffer, ret);

	ret = ewf_string_printf(buffer, sizeof buffer, "%s", "123456789");
	printf("%s (ret = %d)\n", buffer, ret);

	return 0;
}
