#include <stdio.h>

int main(int argc, char **argv)
{
	char *list[] = {"0.0.0.0", "255.255.255.255", "255.255.255.256", "255.255..255", "2d1.255.255.255", "255.255.255.0", "255.255.0.255", "192.0.0.0", "255.252.0.0", "255.255.255.224", "", NULL};
	int n;

	n = 0;
	while (list[n] != NULL)
	{
		if (validate_netmask(list[n]) == 0)
		{
			printf("netmask: %s -> VALID\n", list[n]);
		}
		else
		{
			printf("netmask: %s -> NOT VALID\n", list[n]);
		}

		n++;
	}

	return 0;
}
