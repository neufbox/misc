#include <stdio.h>
#include <string.h>
#include "ewf.h"

int main(int argc, char **argv)
{
	char buffer[8];

	strcpy(buffer, "..");

	if (ewf_string_compare(buffer, sizeof buffer, "..") == EWF_STRING_EQUAL)
	{
		printf("equal\n");
	}
	else
	{
		printf("different\n");
	}

	strcpy(buffer, ".......");

	if (ewf_string_compare(buffer, sizeof buffer, "......") == EWF_STRING_EQUAL)
	{
		printf("equal\n");
	}
	else
	{
		printf("different\n");
	}

	if (ewf_string_compare(buffer, sizeof buffer, ".......") == EWF_STRING_EQUAL)
	{
		printf("equal\n");
	}
	else
	{
		printf("different\n");
	}

	if (ewf_string_compare(buffer, sizeof buffer, "........") == EWF_STRING_EQUAL)
	{
		printf("equal\n");
	}
	else
	{
		printf("different\n");
	}

	return 0;
}
