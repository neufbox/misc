#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>
#include <ewf/core/ewf_renderer.h>

#include "webapp.h"

#include "nbl.h"
#include "nbl_exim.h"
#include "nbd.h"
#include "nbd/spy.h"

PAGE_DF("/maintenance/system", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	ewf_hdf_set("webui_version", WEBUI_VERSION);

	return;
}

FORM_HANDLER
{
	const char *action = NULL;
	FILE *config = NULL;

	action = ewf_hdf_sget("action");
	if (nbu_string_compare(action, "reboot") == NBU_SUCCESS)
	{
		/* Reboot the box */
                ewf_utils_redirect(WEBAPP_URI_REBOOT);
	}
	else if (nbu_string_compare(action, "config_user_export") == NBU_SUCCESS)
	{
		/* Export configuration */
                ewf_renderer_force_raw_output();
                nbl_exim_config_user_export();
	}
	else if (nbu_string_compare(action, "config_user_import") == NBU_SUCCESS)
	{
		/* Import configuration */
		config = ewf_utils_filehandle("config_user_file");
		if (nbl_exim_config_user_import(config) == NBU_SUCCESS)
		{
			ewf_utils_redirect(WEBAPP_URI_REBOOT);
		}
                else
                {
                        ewf_hdf_form_set_error("import_failed");
                }
	}
	else if (nbu_string_compare(action, "config_user_reset") == NBU_SUCCESS)
	{
		/* Send spy event that we are reseting config */
		nbd_spy_event("uireset");

		/* Reset box */
		if (nbd_user_config_erase() == NBD_SUCCESS)
		{
			ewf_utils_redirect(WEBAPP_URI_REBOOT);
		}
	}
}
