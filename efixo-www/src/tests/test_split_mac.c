#include <stdio.h>

int main(int argc, char **argv)
{
	char parts[6][3];
	char *list[] = {"11:22:33:44:55:66", "0:0:0:0:0:0", "FF:FF:FF:FF:FF:FG", "FF:FF:FF:FF::FF", "fff:ff:ff:ff:ff:ff", "Ff:1d:3d:ee:aa:35", "", NULL};
	int n;

	n = 0;
	while (list[n] != NULL)
	{
		if (split_mac(list[n], parts) == 0)
		{
			printf("mac: %s -> %s / %s / %s / %s / %s / %s\n", list[n], parts[0], parts[1], parts[2], parts[3], parts[4], parts[5]);
		}
		else
		{
			printf("mac: %s -> error\n", list[n]);
		}

		n++;
	}

	return 0;
}
