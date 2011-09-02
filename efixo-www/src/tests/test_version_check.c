#include <stdio.h>

#include "../include/ewf/ewf.h"
#include "../include/ewf/core/ewf_theme.h"

#include "nbu/nbu.h"
#include "nbu/list.h"
#include "nbu/log.h"

const char *test_target[] =
{
	"1.1.0 , 5.2.6",
	"1.1.0,5.2.6",
	"1.1.0.5,5.2.6",
	"1.1.0b,5.2.6",
	"1.1.0,1.5",
	"1.1.0,0.9",
	"3.6,9.15.5",
	"3.6.0,9.15.5",
	"3.6.0,9.8",
	NULL,
};

int main(void)
{
	int i = 0;
	while(test_target[i])
	{
		nbu_log_debug("#########################################");
		/* nbu_log_debug("%s <> %s (%d)", WEBUI_VERSION, test_target[i], ewf_theme_check_target(test_target[i])); */

		i++;
	}

	return 0;
}
