#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	char buffer[17];
	char *c;
	int n, p, ret = -1;

	memset(buffer, '\0', 17);
	strncpy(buffer, argv[1], 16);
	n = strlen(buffer);

	/* minimum length is 7 and maximum is 15 */
	if ((n > 6) && (n < 16))
	{
		n = 0;
		p = 0;

		for (c = buffer; *c != '\0'; c++)
		{
			switch(*c)
			{
				case '.':
					if (n > 3)
					{
						/* more than 3 characters before '.' */
						break;
					}

					n = 0;
					p++;

					if (p == 3)
					{
						if (isdigit(*++c) == 0)
							return -1;
					}

					break;

				default:
					if (isdigit(*c) == 0)
					n++;
					break;
			}
		}

		if(p != 3)
			return -1;
	}

	if (ret == 0)
	{
		printf("ok\n");
	}
	else
	{
		printf("nok\n");
	}

	return ret;
}
