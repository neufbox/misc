/*! \file    getmac.c
 *  \brief   Return board mac, with offset asked.
 *  \author  Arnaud Rebillout, arnaud.rebillout@efixo.com
 *  \version 0.1
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <arpa/inet.h>

static void parse_option(int argc, char **argv, char const **format,
			 long *offset)
{
	/* load default */
	*format = ":";
	*offset = 0;

	for (;;) {
		int opt = getopt(argc, argv, "svp");

		if (opt == -1) {
			break;
		} else if (opt == 's') {
			*format = ":";
			break;
		} else if (opt == 'v') {
			*format = "-";
			break;
		} else if (opt == 'p') {
			*format = "";
			break;
		} else {
			fprintf(stderr,
				"usage: %s [-s (system)] [-v (voip)] [-p (ppp)] <offset>\n",
				argv[0]);
			exit(EXIT_SUCCESS);
		}
	}

	if ((argc >= 2) && argv[optind]) {
		*offset = strtol(argv[optind], NULL, 10);
	}
}

int main(int argc, char *argv[])
{
	unsigned buf[6];
	uint8_t mac[6];
	uint32_t i;
	FILE *fp;
	char const *format;
	long offset;

	parse_option(argc, argv, &format, &offset);
	/* Get mac from proc fs */
	fp = fopen("/proc/mac_address_base", "r");
	if (!fp) {
		fprintf(stderr, "%s(%s)\n", "fopen", "/proc/mac_address_base");
		return EXIT_FAILURE;
	}
	if (fscanf(fp, "%02X%02X%02X%02X%02X%02X",
		   buf, buf + 1, buf + 2, buf + 3, buf + 4, buf + 5) < 6) {
		fprintf(stderr, "%s(%s, %s)\n", "fscanf",
			"/proc/mac_address_base", "%02X%02X%02X%02X%02X%02X");
		return EXIT_FAILURE;
	}
	fclose(fp);

	for (i = 0; i < 6; ++i) {
		mac[i] = buf[i];
	}

	/* Set mac offset */
	memcpy(&i, mac + 2, sizeof(i));
	i = ntohl(i);
	i += offset;
	i = htonl(i);
	memcpy(mac + 2, &i, sizeof(i));

	printf("%02X%s%02X%s%02X%s%02X%s%02X%s%02X\n",
	       mac[0], format, mac[1], format,
	       mac[2], format, mac[3], format, mac[4], format, mac[5]);

	return EXIT_SUCCESS;
}
