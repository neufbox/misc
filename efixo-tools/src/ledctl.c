/*!
 * leds.c
 *
 * Copyright 2006 Miguel GAIO <miguel.gaio@efixo.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * 
 * $Id: ledctl.c 17551 2010-09-14 07:08:59Z mgo $
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <neufbox/leds.h>

#define ARRAY_SIZE(x) (sizeof (x) / sizeof ((x) [0]))

static char const *const led_name[] = {
#ifndef NB6
	"wan",
	"traffic",
	"tel",
	"tv",
	"wifi",
	"alarm",
	"red",
	"green",
	"blue",
	"link",
	"speed",
	"logo",
	NULL
#else
	"service1:green",
	"service1:red",
	"service2:green",
	"service2:red",
	"service3:green",
	"service3:red",
	"voip",
	"wan",
	"wlan",
#endif
};

static int leds_usage(void)
{
	fprintf(stderr, "usage: ledctl\n"
#ifdef NB6
		"            mode [disable|boot|boot-main|boot-tftp|boot-rescue|login|burning|downloading|wdt-temperature|wdt-voltage|panic|control]\n"
#else
		"            mode [disable|control|downloading|burning|panic|demo]\n"
#endif
		"            <led id> [unchanged|on|off|toggle|blinkonce|blinks]\n"
		"            list\n");

	return 1;
}

#ifndef NB6
static int leds_ioctl(unsigned int cmd, struct leds_dev_ioctl_struct *led_ioctl)
{
	int fd;

	if ((fd = open("/dev/leds", O_RDWR)) < 0) {
		fprintf(stderr, "open(/dev/leds, O_RDWR): %s ",
			strerror(errno));
		return 1;
	}

	if (ioctl(fd, cmd, led_ioctl) < 0) {
		fprintf(stderr, "ioctl(%d, %d, %p): %s ", fd, cmd,
			led_ioctl, strerror(errno));
		close(fd);
		return 1;
	}

	close(fd);

	return 0;
}

static int leds_mode(char const *arg)
{
	struct leds_dev_ioctl_struct leds_dev_ioctl;

	if (arg == NULL) {
		if (leds_ioctl(LED_IOCTL_GET_MODE, &leds_dev_ioctl) < 0) {
			return 1;
		}

		switch (leds_dev_ioctl.mode) {
		case led_mode_disable:
			puts("disable");
			break;

		case led_mode_control:
			puts("control");
			break;

		case led_mode_downloading:
			puts("downloading");
			break;

		case led_mode_burning:
			puts("burning");
			break;

		case led_mode_panic:
			puts("panic");
			break;

		case led_mode_demo:
			puts("demo");
			break;

		default:
			printf("unkown mode %d\n", leds_dev_ioctl.mode);
			return 1;
		}

		return 0;
	}

	if (strcmp(arg, "disable") == 0) {
		leds_dev_ioctl.mode = led_mode_disable;
	} else if (strcmp(arg, "control") == 0) {
		leds_dev_ioctl.mode = led_mode_control;
	} else if (strcmp(arg, "downloading") == 0) {
		leds_dev_ioctl.mode = led_mode_downloading;
	} else if (strcmp(arg, "burning") == 0) {
		leds_dev_ioctl.mode = led_mode_burning;
	} else if (strcmp(arg, "panic") == 0) {
		leds_dev_ioctl.mode = led_mode_panic;
	} else if (strcmp(arg, "demo") == 0) {
		leds_dev_ioctl.mode = led_mode_demo;
	} else {
		return leds_usage();
	}

	return leds_ioctl(LED_IOCTL_SET_MODE, &leds_dev_ioctl);
}

static int leds_list(void)
{
	unsigned int id;
	struct leds_dev_ioctl_struct leds_dev_ioctl;

	char const *const led_name_states[] = {
		"unchanged",
		"off",
		"on",
		"toggle",
		"blinkonce",
		"slowblinks",
		"blinks"
	};

	for (id = 0; id < led_id_last; ++id) {
		leds_dev_ioctl.id = id;
		if (!leds_ioctl(LED_IOCTL_GET_LED, &leds_dev_ioctl))
			printf("%s=%s\n", led_name[id],
			       led_name_states[leds_dev_ioctl.state]);
		else
			printf("%s : error while obtaining state !\n",
			       led_name[id]);
	}
	return 0;
}

static int leds(char const *id, char const *state)
{
	struct leds_dev_ioctl_struct leds_dev_ioctl;

	if (id == NULL) {
		return leds_usage();
	}

	if (strcmp(id, "wan") == 0) {
		leds_dev_ioctl.id = led_id_wan;
	} else if (strcmp(id, "traffic") == 0) {
		leds_dev_ioctl.id = led_id_traffic;
	} else if (strcmp(id, "tel") == 0) {
		leds_dev_ioctl.id = led_id_voip;
	} else if (strcmp(id, "tv") == 0) {
		leds_dev_ioctl.id = led_id_tv;
	} else if (strcmp(id, "wifi") == 0) {
		leds_dev_ioctl.id = led_id_wlan;
	} else if (strcmp(id, "alarm") == 0) {
		leds_dev_ioctl.id = led_id_alarm;
	} else if (strcmp(id, "red") == 0) {
		leds_dev_ioctl.id = led_id_red;
	} else if (strcmp(id, "green") == 0) {
		leds_dev_ioctl.id = led_id_green;
	} else if (strcmp(id, "blue") == 0) {
		leds_dev_ioctl.id = led_id_blue;
	} else {
		return leds_usage();
	}

	if (state == NULL) {
		if (leds_ioctl(LED_IOCTL_GET_LED, &leds_dev_ioctl) < 0) {
			return 1;
		}

		switch (leds_dev_ioctl.state) {
		case led_state_on:
			puts("on");
			break;

		case led_state_off:
			puts("off");
			break;

		case led_state_toggle:
			puts("toggle");
			break;

		case led_state_blinkonce:
			puts("blinkonce");
			break;

		case led_state_slowblinks:
			puts("slowblinks");
			break;

		case led_state_blinks:
			puts("blinks");
			break;

		default:
			printf("unkown led state %d\n", leds_dev_ioctl.state);
			return 1;
		}

		return 0;
	}

	if (strcmp(state, "on") == 0) {
		leds_dev_ioctl.state = led_state_on;
	} else if (strcmp(state, "off") == 0) {
		leds_dev_ioctl.state = led_state_off;
	} else if (strcmp(state, "toggle") == 0) {
		leds_dev_ioctl.state = led_state_toggle;
	} else if (strcmp(state, "blinkonce") == 0) {
		leds_dev_ioctl.state = led_state_blinkonce;
	} else if (strcmp(state, "slowblinks") == 0) {
		leds_dev_ioctl.state = led_state_slowblinks;
	} else if (strcmp(state, "blinks") == 0) {
		leds_dev_ioctl.state = led_state_blinks;
	} else {
		return leds_usage();
	}

	return leds_ioctl(LED_IOCTL_SET_LED, &leds_dev_ioctl);
}
#else
static int leds_mode(char const *arg)
{
	char buf[256];
	char *s, *e;
	FILE *fp;

	if (chdir("/sys/class/hwmon/hwmon0/device/") < 0) {
		fprintf(stderr, "%s(%s) %m", "chdir",
			"/sys/class/hwmon/hwmon0/device/");
		return 1;
	}

	if (arg == NULL) {
		fp = fopen("leds_mode", "r");
		if (!fp) {
			fprintf(stderr, "%s(%s) %m", "fopen", "leds_mode");
			return 1;
		}

		fread(buf, 1, sizeof(buf), fp);
		s = strchr(buf, '[');
		if (!s)
			return 1;
		++s;
		e = strchr(s, ']');
		if (!e)
			return 1;
		*e = '\0';
		printf("%s\n", s);
		return 0;
	}

	fp = fopen("leds_mode", "w");
	if (!fp) {
		fprintf(stderr, "%s(%s) %m", "fopen", "leds_mode");
		return 1;
	}
	fprintf(fp, "%s", arg);
	fclose(fp);

	return 0;
}

static char const *level_str(unsigned level)
{
	if (level & 0x80)
		return "blinks";
	else if (level)
		return "on";
	else
		return "off";
}

static unsigned led_level(char const *led)
{
	FILE *fp;
	unsigned level;

	if (chdir(led) < 0) {
		fprintf(stderr, "%s(%s) %m", "chdir", led);
		exit(EXIT_FAILURE);
	}

	fp = fopen("brightness", "r");
	if (!fp) {
		fprintf(stderr, "%s(%s)", "fopen", "brightness");
		exit(EXIT_FAILURE);
	}
	fscanf(fp, "%u", &level);
	fclose(fp);

	if (chdir("..") < 0) {
		fprintf(stderr, "%s(%s) %m", "chdir", "..");
		exit(EXIT_FAILURE);
	}

	return level;
}

static unsigned led_set_level(char const *led, unsigned level)
{
	FILE *fp;

	if (chdir(led) < 0) {
		fprintf(stderr, "%s(%s) %m", "chdir", led);
		exit(EXIT_FAILURE);
	}

	fp = fopen("brightness", "w");
	if (!fp) {
		fprintf(stderr, "%s(%s)", "fopen", "brightness");
		exit(EXIT_FAILURE);
	}
	fprintf(fp, "%u", level);
	fclose(fp);

	if (chdir("..") < 0) {
		fprintf(stderr, "%s(%s) %m", "chdir", "..");
		exit(EXIT_FAILURE);
	}

	return level;
}

static int leds_list(void)
{
	unsigned i;
	char const *led;

	if (chdir("/sys/class/leds/") < 0) {
		fprintf(stderr, "%s(%s) %m", "chdir", "/sys/class/leds/");
		return 1;
	}

	for (i = 0; i < ARRAY_SIZE(led_name); ++i) {
		led = led_name[i];
		printf("%s=%s\n", led, level_str(led_level(led)));
	}
	return 0;
}

static int leds(char const *led, char const *level)
{
	FILE *fp;
	unsigned i;

	if (led == NULL) {
		return leds_usage();
	}

	if (chdir("/sys/class/leds/") < 0) {
		fprintf(stderr, "%s(%s) %m", "chdir", "/sys/class/leds/");
		return 1;
	}

	if (level == NULL) {
		printf("%s\n", level_str(led_level(led)));
		return 0;
	}

	if (strcmp(level, "on") == 0) {
		led_set_level(led, 31);
	} else if (strcmp(level, "off") == 0) {
		led_set_level(led, 0);
	} else if ((strcmp(level, "slowblinks") == 0)
		   || (strcmp(level, "blinks") == 0)) {
		led_set_level(led, 0x80 | 31);
	} else {
		return leds_usage();
	}

	return 0;
}
#endif

int main(int argc, char *argv[])
{
	if (argc < 2) {
		return leds_usage();
	}

	if (strcmp(argv[1], "mode") == 0) {
		return leds_mode(argv[2]);
	}

	if (strcmp(argv[1], "list") == 0) {
		return leds_list();
	}

	return leds(argv[1], argv[2]);
}
