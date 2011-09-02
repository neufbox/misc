
#include <stdint.h>
#include <string.h>
#include <etk/log.h>

static uint8_t hex(char c)
{
	/* convert to lower */
	c |= 0x20;

	if ((c >= '0') && (c <= '9')) {
		return c - '0';
	} else if ((c >= 'a') && (c <= 'f')) {
		return c - 'a' + 10;
	} else {
		err("invalid hexa digit: %02xh (\'%c\')", c, c);
		return 0xff;
	}
}

void pack(char const *s, unsigned char *pck, size_t len)
{
	memset(pck, 0x00, len);

	while ((*s != '\0') && len) {
		*pck = (hex(*s) << 4), ++s;
		*pck |= hex(*s), ++s, ++pck, --len;
	}
}
