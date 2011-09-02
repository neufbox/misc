#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbd.h"

#include "kit_helper.h"

PAGE_DF("/kit/action", EWF_ACCESS_PUBLIC);

DISPLAY_HANDLER
{

}

FORM_HANDLER
{
	int ret;
	const char *action = NULL,
                *script = NULL;

	action = ewf_hdf_sget("action");
	script = ewf_hdf_sget("script");

	if (action != NULL && script != NULL)
	{
		if((ret = kit_helper_action(script, action)) == 0)
		{
			nbu_log_info("%s %s SUCCESS", script, action);
		}
		else
		{
			nbu_log_error("%s %s FAILED (REALY or UNAUTHORIZED ? - %d)",
				      script, action, ret);
		}
	}

	return;
}
