#include <stdio.h>

int main(int argc, char **argv)
{
	char *list[] = {"192.168.9.1", "0.0.0.0", "255.255.255.256", "255.255..255", "2d1.255.255.255", "", NULL};
	int n;

	n = 0;
	while (list[n] != NULL)
	{
		if (validate_ip(list[n]) == 0)
		{
			printf("ip: %s -> VALID\n", list[n]);
		}
		else
		{
			printf("ip: %s -> NOT VALID\n", list[n]);
		}

		n++;
	}

	return 0;
}
