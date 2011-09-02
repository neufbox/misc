#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbd.h"

#include "kit_helper.h"

PAGE_DF("/kit/get", EWF_ACCESS_PUBLIC);

DISPLAY_HANDLER
{

}

FORM_HANDLER
{
	const char *name = NULL;

	name = ewf_hdf_sget("name");

	if(name != NULL
	   && kit_helper_get(name, buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
                ewf_hdf_set("name", name);
                ewf_hdf_set("value", buffer);
		//ewf_fcgi_printf("%s=%s\n", name, buffer);
	}

	return;
}
