#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

PAGE_DF("/maintenance/dsl", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	/* default */
	ewf_hdf_set("ADSLProfile", "auto");

	if (nbd_user_config_get("adsl_profile", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		if (nbu_string_matches(buffer, "0") == NBU_STRING_EQUAL)
		{
			ewf_hdf_owset("ADSLProfile", "manual");
		}
		else if (nbu_string_matches(buffer, "6") == NBU_STRING_EQUAL)
		{
			ewf_hdf_owset("ADSLProfile", "rescue");
		}
		else
		{
			ewf_hdf_owset("ADSLProfile", "auto");
		}
	}

	if (nbd_user_config_get("adsl_userprofile_mod", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		/* default */
		ewf_hdf_set("ADSLProfileNumber", "1");

		if (nbu_string_compare(buffer, "dlt") == NBU_SUCCESS)
		{
			if (nbd_user_config_get("adsl_userprofile_snr", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				if (nbu_string_compare(buffer, "224") == NBU_SUCCESS)
				{
					ewf_hdf_owset("ADSLProfileNumber", "1");
				}
			}
		}
		else if (nbu_string_compare(buffer, "adlt2pe") == NBU_SUCCESS)
		{
			if (nbd_user_config_get("adsl_userprofile_snr", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				if (nbu_string_compare(buffer, "224") == NBU_SUCCESS)
				{
					ewf_hdf_owset("ADSLProfileNumber", "2");
				}
				else if (nbu_string_compare(buffer, "192") == NBU_SUCCESS)
				{
					ewf_hdf_owset("ADSLProfileNumber", "3");
				}
				else if (nbu_string_compare(buffer, "160") == NBU_SUCCESS)
				{
					ewf_hdf_owset("ADSLProfileNumber", "4");
				}
				else if (nbu_string_compare(buffer, "128") == NBU_SUCCESS)
				{
					ewf_hdf_owset("ADSLProfileNumber", "5");
				}
				else if (nbu_string_compare(buffer, "96") == NBU_SUCCESS)
				{
					ewf_hdf_owset("ADSLProfileNumber", "6");
				}
			}
		}
	}

	return;
}

FORM_HANDLER
{
	int restart = 0;
	const char *profile = NULL, *profile_number = NULL;
        char *oldprofile = NULL;

	if (nbd_user_config_get("adsl_oldprofile", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		oldprofile = strdup(buffer);
	}

	profile = ewf_hdf_sget("adsl_profile");
	profile_number = ewf_hdf_sget("adsl_profile_n");

	if (nbu_string_compare(profile, "auto") == NBU_SUCCESS)
	{
		if (nbd_user_config_set("adsl_profile", oldprofile) == NBD_SUCCESS)
		{
			restart++;
		}
	}
	else if (nbu_string_compare(profile, "manual") == NBU_SUCCESS)
	{
		/* If profile is neither manual nor rescue save actual profile into oldprofile */
		if (nbd_user_config_get("adsl_profile", buffer, sizeof(buffer)) == NBD_SUCCESS)
		{
			if ( nbu_string_compare(buffer, "0") != NBU_SUCCESS &&
			     nbu_string_compare(buffer, "6") != NBU_SUCCESS )
				nbd_user_config_set("adsl_oldprofile", buffer);
		}

		if (nbd_user_config_set("adsl_profile", "0") == NBD_SUCCESS)
		{
			restart++;
		}

		if (nbu_string_compare(profile_number, "1") == NBU_SUCCESS)
		{
			if ( nbd_user_config_set("adsl_userprofile_mod", "dlt") == NBD_SUCCESS)
				restart++;
			if ( nbd_user_config_set("adsl_userprofile_snr", "224") == NBD_SUCCESS)
				restart++;
		}
		else if (nbu_string_compare(profile_number, "2") == NBU_SUCCESS)
		{
			if ( nbd_user_config_set("adsl_userprofile_mod", "adlt2pe") == NBD_SUCCESS)
				restart++;
			if ( nbd_user_config_set("adsl_userprofile_snr", "224") == NBD_SUCCESS)
				restart++;
		}
		else if (nbu_string_compare(profile_number, "3") == NBU_SUCCESS)
		{
			if ( nbd_user_config_set("adsl_userprofile_mod", "adlt2pe") == NBD_SUCCESS)
				restart++;
			if ( nbd_user_config_set("adsl_userprofile_snr", "192") == NBD_SUCCESS)
				restart++;
		}
		else if (nbu_string_compare(profile_number, "4") == NBU_SUCCESS)
		{
			if ( nbd_user_config_set("adsl_userprofile_mod", "adlt2pe") == NBD_SUCCESS)
				restart++;
			if ( nbd_user_config_set("adsl_userprofile_snr", "160") == NBD_SUCCESS)
				restart++;
		}
		else if (nbu_string_compare(profile_number, "5") == NBU_SUCCESS)
		{
			if ( nbd_user_config_set("adsl_userprofile_mod", "adlt2pe") == NBD_SUCCESS)
				restart++;
			if ( nbd_user_config_set("adsl_userprofile_snr", "128") == NBD_SUCCESS)
				restart++;
		}
		else if (nbu_string_compare(profile_number, "6") == NBU_SUCCESS)
		{
			if ( nbd_user_config_set("adsl_userprofile_mod", "adlt2pe") == NBD_SUCCESS)
				restart++;
			if ( nbd_user_config_set("adsl_userprofile_snr", "96") == NBD_SUCCESS)
				restart++;
		}

	}
	else if (nbu_string_compare(profile, "rescue") == NBU_SUCCESS)
	{
		/* If profile is neither manual nor rescue save actual profile into oldprofile */
		if (nbd_user_config_get("adsl_profile", buffer, sizeof(buffer)) == NBD_SUCCESS)
		{
			if ( nbu_string_compare(buffer, "0") != NBU_SUCCESS &&
			     nbu_string_compare(buffer, "6") != NBU_SUCCESS )
				nbd_user_config_set("adsl_oldprofile", buffer);
		}

		if (nbd_user_config_set("adsl_profile", "6") == NBD_SUCCESS)
		{
			restart++;
		}
	}

        if(oldprofile != NULL)
        {
                free(oldprofile);
        }

	if (restart > 0)
	{
		nbd_user_config_commit();
#ifndef X86
		nbd_dsl_restart();
#endif
	}
}
