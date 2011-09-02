#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *mac;

	if (join_mac("0", "1", "2", "3", "4", "5", mac) == 0)
	{
		printf("mac: %s\n", mac);
	}
	else
	{
		printf("error: join_mac(\"0\", \"1\", \"2\", \"3\", \"4\", \"5\", mac)\n");
	}

	if (join_mac("0", "0", "0", "0", "0", "", mac) == 0)
	{
		printf("mac: %s\n", mac);
	}
	else
	{
		printf("error: join_mac(\"0\", \"0\", \"0\", \"0\", \"0\", \"\", mac)\n");
	}

	if (join_mac("FG", "FF", "FF", "FF", "FF", "FF", mac) == 0)
	{
		printf("mac: %s\n", mac);
	}
	else
	{
		printf("error: join_mac(\"FG\", \"FF\", \"FF\", \"FF\", \"FF\", \"FF\", mac)\n");
	}

	if (join_mac("111", "FF", "FF", "FF", "FF", "FF", mac) == 0)
	{
		printf("mac: %s\n", mac);
	}
	else
	{
		printf("error: join_mac(\"111\", \"FF\", \"FF\", \"FF\", \"FF\", \"FF\", mac)\n");
	}

	if (join_mac("a", "1", "22", "33", "aA", "FF", mac) == 0)
	{
		printf("mac: %s\n", mac);
	}
	else
	{
		printf("error: join_mac(\"a\", \"1\", \"22\", \"33\", \"aA\", \"FF\", mac)\n");
	}

	return 0;
}
