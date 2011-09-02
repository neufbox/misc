#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

PAGE_D("/maintenance", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	/* Protection */
	if(nbd_user_config_get("web_auth", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("web_auth", buffer);
	}

	/* Theme */
	//nv_to_hdf("web_theme_dirname", "WebThemeName");

#ifndef NB5
	/* ADSL profil */
	ewf_hdf_set("ADSLProfile", "auto");

	if (nbd_user_config_get("adsl_profile", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		if (nbu_string_compare(buffer, "0") == NBU_SUCCESS)
		{
			ewf_hdf_owset("ADSLProfile", "manual");
		}
		else if (nbu_string_compare(buffer, "6") == NBU_SUCCESS)
		{
			ewf_hdf_owset("ADSLProfile", "rescue");
		}
		else
		{
			ewf_hdf_owset("ADSLProfile", "auto");
		}
	}
#endif

	return;
}
