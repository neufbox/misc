#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *ip;

	if (join_ip("192", "168", "9", "1", ip) == 0)
	{
		printf("ip: %s\n", ip);
	}
	else
	{
		printf("error: join_ip(\"192\", \"168\", \"9\", \"1\", ip)\n");
	}

	if (join_ip("0", "0", "0", "0", ip) == 0)
	{
		printf("ip: %s\n", ip);
	}
	else
	{
		printf("error: join_ip(\"0\", \"0\", \"0\", \"0\", ip)\n");
	}

	if (join_ip("284", "255", "255", "255", ip) == 0)
	{
		printf("ip: %s\n", ip);
	}
	else
	{
		printf("error: join_ip(\"284\", \"255\", \"255\", \"255\", ip)\n");
	}

	if (join_ip("a", "255", "255", "255", ip) == 0)
	{
		printf("ip: %s\n", ip);
	}
	else
	{
		printf("error: join_ip(\"a\", \"255\", \"255\", \"255\", ip)\n");
	}

	return 0;
}
