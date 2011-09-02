#include <stdio.h>
#include <string.h>
#include "ewf.h"

int main(int argc, char **argv)
{
	char buffer[8];

	strcpy(buffer, ".......");
	strcpy(buffer + sizeof buffer, "aaaaaaaa");
	printf("buffer = %s\n", buffer);
	printf("strlen           : %1$d (buffer[%1$d] = 0x%2$02x)\n", strlen(buffer), buffer[strlen(buffer)]);
	printf("strnlen          : %1$d (buffer[%1$d] = 0x%2$02x)\n", strnlen(buffer, sizeof buffer), buffer[strnlen(buffer, sizeof buffer)]);
	printf("ewf_string_length: %1$d (buffer[%1$d] = 0x%2$02x)\n", ewf_string_length(buffer, sizeof buffer), buffer[ewf_string_length(buffer, sizeof buffer)]);
	printf("\n");

	strcpy(buffer, "........");
	strcpy(buffer + sizeof buffer, "aaaaaaaa");
	printf("buffer = %s\n", buffer);
	printf("strlen           : %1$d (buffer[%1$d] = 0x%2$02x)\n", strlen(buffer), buffer[strlen(buffer)]);
	printf("strnlen          : %1$d (buffer[%1$d] = 0x%2$02x)\n", strnlen(buffer, sizeof buffer), buffer[strnlen(buffer, sizeof buffer)]);
	printf("ewf_string_length: %1$d (buffer[%1$d] = 0x%2$02x)\n", ewf_string_length(buffer, sizeof buffer), buffer[ewf_string_length(buffer, sizeof buffer)]);
	printf("\n");

	strcpy(buffer, ".........");
	strcpy(buffer + sizeof buffer, "aaaaaaaa");
	printf("buffer = %s\n", buffer);
	printf("strlen           : %1$d (buffer[%1$d] = 0x%2$02x)\n", strlen(buffer), buffer[strlen(buffer)]);
	printf("strnlen          : %1$d (buffer[%1$d] = 0x%2$02x)\n", strnlen(buffer, sizeof buffer), buffer[strnlen(buffer, sizeof buffer)]);
	printf("ewf_string_length: %1$d (buffer[%1$d] = 0x%2$02x)\n", ewf_string_length(buffer, sizeof buffer), buffer[ewf_string_length(buffer, sizeof buffer)]);
	printf("\n");

	return 0;
}
