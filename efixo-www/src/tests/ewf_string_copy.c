#include <stdio.h>
#include <string.h>
#include "ewf.h"

/* libc6 2.7-3 debian
 * uClibc 0.9.27
 */
int main(int argc, char **argv)
{
	char buffer[16];
	char fill[16] = "...............";

	printf("* strncpy(NULL, \"123456\", 7)\n");
	memcpy(buffer, fill, 16);
	/* strncpy(NULL, "123456", 7); */
	printf("strncpy        : segfault\n");
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(NULL, 7, "123456") == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	printf("* strncpy(buffer, NULL, 7)\n");
	memcpy(buffer, fill, 16);
	/* strncpy(buffer, NULL, 7); */
	printf("strncpy        : segfault\n");
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(buffer, 7, NULL) == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	printf("* strncpy(buffer, \"123456\", -1)\n");
	memcpy(buffer, fill, 16);
	/* strncpy(buffer, "123456", -1); */
	printf("strncpy        : segfault\n");
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(buffer, -1, "123456") == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	printf("* strncpy(buffer, \"123456\", 0)\n");
	memcpy(buffer, fill, 16);
	strncpy(buffer, "123456", 0);
	printf("strncpy        : %s\n", buffer);
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(buffer, 0, "123456") == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	printf("* strncpy(buffer, \"123456\", 5)\n");
	memcpy(buffer, fill, 16);
	strncpy(buffer, "123456", 5);
	printf("strncpy        : %s\n", buffer);
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(buffer, 5, "123456") == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	printf("* strncpy(buffer, \"123456\", 6)\n");
	memcpy(buffer, fill, 16);
	strncpy(buffer, "123456", 6);
	printf("strncpy        : %s\n", buffer);
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(buffer, 6, "123456") == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	printf("* strncpy(buffer, \"123456\", 7)\n");
	memcpy(buffer, fill, 16);
	strncpy(buffer, "123456", 7);
	printf("strncpy        : %s\n", buffer);
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(buffer, 7, "123456") == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	printf("* strncpy(buffer, \"0123456789abcde\", sizeof buffer - 1)\n");
	memcpy(buffer, fill, 16);
	strncpy(buffer, "0123456789abcde", sizeof buffer - 1);
	printf("strncpy        : %s\n", buffer);
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(buffer, sizeof buffer - 1, "0123456789abcde") == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	printf("* strncpy(buffer, \"0123456789abcde\", sizeof buffer)\n");
	memcpy(buffer, fill, 16);
	strncpy(buffer, "0123456789abcde", sizeof buffer);
	if (buffer[sizeof buffer - 1] != '\0')
	{
		printf("strncpy        : overflow\n");
	}
	else
	{
		printf("strncpy        : %s\n", buffer);
	}
	memcpy(buffer, fill, 16);
	if (ewf_string_copy(buffer, sizeof buffer, "0123456789abcde") == EWF_SUCCESS)
	{
		printf("ewf_string_copy: %s\n", buffer);
	}
	else
	{
		printf("ewf_string_copy: error\n");
	}
	printf("\n");

	return 0;
}
