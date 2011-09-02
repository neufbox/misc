#include <stdio.h>

int main(int argc, char **argv)
{
	char parts[4][4];
	char *list[] = {"192.168.9.1", "0.0.0.0", "255.255.255.256", "255.255..255", "2d1.255.255.255", "", NULL};
	int n;

	n = 0;
	while (list[n] != NULL)
	{
		if (split_ip(list[n], parts) == 0)
		{
			printf("ip: %s -> %s / %s / %s / %s\n", list[n], parts[0], parts[1], parts[2], parts[3]);
		}
		else
		{
			printf("ip: %s -> error\n", list[n]);
		}

		n++;
	}

	return 0;
}
