#ifndef _NBL_LEDS_H_
#define _NBL_LEDS_H_

#include "nbd/leds.h"

static inline const char * nbl_leds_state_cstr(enum led_state state)
{
	char* ret = NULL;

	if(state == led_state_off)
	{
		ret = "off";
	}
	else if(state == led_state_on)
	{
		ret = "on";
	}
	else if(state == led_state_unchanged)
	{
		ret = "unchanged";
	}
	else if(state == led_state_toggle)
	{
		ret = "toggle";
	}
	else
	{
		/* blink status */
		if(rand() % 2 == 0)
		{
			ret = "on";
		}
		else
		{
			ret = "off";
		}
	}

	return ret;
}

static inline const char * nbl_leds_brightness_cstr(void)
{
	if (nbd_leds_get_mode() == led_mode_disable )
	{
		return "off";
	}
	else
	{
		return "on";
	}
}

#endif
