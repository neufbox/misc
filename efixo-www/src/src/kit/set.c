#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbd.h"

#include "kit_helper.h"

PAGE_DF("/kit/set", EWF_ACCESS_PUBLIC);

DISPLAY_HANDLER
{

}

FORM_HANDLER
{
	const char *name = NULL,
                *value = NULL;
	int ret;

	name = ewf_hdf_sget("name");
	value = ewf_hdf_sget("value");

	if ((name != NULL) && (value != NULL))
	{
		ret = kit_helper_set(name, value);

                ewf_hdf_set("name", name);

		if (ret == NV_SET_SUCCESS)
		{
                        nbd_user_config_commit();
                        ewf_hdf_set("status", "OK");
			//ewf_fcgi_printf("%s=OK\n", name);
		}
		else if (ret == NV_SET_UNCHANGED)
		{
                        ewf_hdf_set("status", "NC");
		}
		else
		{
                        ewf_hdf_set("status", "KO");
		}
	}

	return;
}
