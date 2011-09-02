#include <signal.h>
#include <libgen.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>
#include <ewf/ewf_crypto.h>
#include <ewf/core/ewf_config.h>
#include <ewf/core/ewf_theme.h>
#include <ewf/core/ewf_fcgi.h>

#include <nbu/file.h>

#include "nbl.h"
#include "nbd.h"
#include "dbl.h"

PAGE_DF("/maintenance/webui", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        char curr_uri_theme[256];

	const char* uri_theme = NULL;
        int ret;

	int selected_theme_is_present = 0;

	ewf_theme_ipkg_t* theme_ipkg = NULL;
	int m = 0;
	nbu_list_t* theme_list = NULL;

	/**************/
	/* theme part */
	/**************/

        uri_theme = ewf_hdf_sget("uri_theme");
	if(uri_theme != NULL)
	{
		/* User want to change theme */
		if((ret = ewf_theme_set(uri_theme)) == EWF_THEME_LOAD_SUCCESS)
		{
                        ewf_utils_redirect("/maintenance/webui");
                        return;
                }
                else
                {
			nbu_log_error("Error when setting theme uri='%s'",
				      uri_theme);

                        switch(ret)
                        {
                        case EWF_THEME_LOAD_UNKNOWN_SCHEME:
                        case EWF_THEME_LOAD_INVALID_URI:
                                ewf_hdf_form_set_error("invalid_uri");
                        case EWF_THEME_LOAD_INSTALL_FAILED:
                                ewf_hdf_form_set_error("install_failed");
                        case EWF_THEME_LOAD_ERROR:
                        default:
                                ewf_hdf_form_set_error("unknown");
                        }
		}
	}

        if(ewf_config_get("web_theme_uri",
                          curr_uri_theme,
                          sizeof(curr_uri_theme)) == EWF_SUCCESS)
        {
                ewf_hdf_set("current_theme_uri", curr_uri_theme);
                ewf_hdf_set("current_theme_name",
                            ewf_theme_uri_basename(curr_uri_theme));
        }

	/* Theme list */
	if(ewf_theme_get_list(&theme_list) == EWF_SUCCESS)
	{
		m = 0;
		nbu_list_for_each_data(theme_list, theme_ipkg, ewf_theme_ipkg_t)
		{
                        if(!selected_theme_is_present
                           && nbu_string_compare(theme_ipkg->uri,
                                                 curr_uri_theme)
                           == NBU_STRING_EQUAL)
                        {
                                selected_theme_is_present = 1;
                                nbu_string_printf(buffer, sizeof buffer,
                                                  "Themes.%d.selected", m);
                                ewf_hdf_set_int(buffer, 1);
                        }

                        if(theme_ipkg->has_preview)
                        {
                                nbu_string_printf(buffer, sizeof buffer,
                                                  "Themes.%d.has_preview", m);
                                ewf_hdf_set_int(buffer, 1);
                        }

                        if(theme_ipkg->is_compatible)
                        {
                                nbu_string_printf(buffer, sizeof buffer,
                                                  "Themes.%d.is_compatible", m);
                                ewf_hdf_set_int(buffer, 1);
                        }

			nbu_string_printf(buffer, sizeof buffer,
                                          "Themes.%d.uri", m);
			ewf_hdf_set(buffer, theme_ipkg->uri);

			nbu_string_printf(buffer, sizeof buffer,
                                          "Themes.%d.uuid", m);
			ewf_hdf_set(buffer, theme_ipkg->theme.uuid);

			nbu_string_printf(buffer, sizeof buffer,
                                          "Themes.%d.name", m);
			ewf_hdf_set(buffer, theme_ipkg->theme.name);

			nbu_string_printf(buffer, sizeof buffer,
                                          "Themes.%d.long_name", m);
			ewf_hdf_set(buffer, theme_ipkg->theme.longname);

			nbu_string_printf(buffer, sizeof buffer,
                                          "Themes.%d.version", m);
			ewf_hdf_set(buffer, theme_ipkg->theme.version);

			nbu_string_printf(buffer, sizeof buffer,
                                          "Themes.%d.author", m);
			ewf_hdf_set(buffer, theme_ipkg->theme.author);

			nbu_string_printf(buffer, sizeof buffer,
                                          "Themes.%d.description", m);
			ewf_hdf_set(buffer, theme_ipkg->theme.description);

			nbu_string_printf(buffer, sizeof buffer,
                                          "Themes.%d.www", m);
			ewf_hdf_set(buffer, theme_ipkg->theme.homepage);

			m++;
		}

		nbu_list_destroy(theme_list);
	}

	if(!selected_theme_is_present)
	{
		/* theme selected is not present, get information from nvram
		 * and put it in hdf */
		ewf_hdf_set("current_theme_present", "0");
	}

	/*********************/
	/* localization part */
	/*********************/

	nv_to_hdf("web_lang", "lang");

	return;
}

FORM_HANDLER
{
        const char *lang = NULL;
        struct trx trx;

        EWF_HDF_GET(lang);

        NV_TRX_INIT(trx);

        NV_SET_WTRX(trx, "web_lang", lang);

        if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
        {
                raise(SIGHUP);

                /* all fine ! redirect to make change visible */
                ewf_utils_redirect("/maintenance/webui");
        }

	return;
}
