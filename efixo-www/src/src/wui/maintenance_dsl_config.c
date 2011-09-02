#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>

#include "nbl.h"
#include "nbd.h"

PAGE_DF("/maintenance/dsl/config", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        if (nbd_dsl_mod_get(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("DslMod", buffer);
        }

        if (nbd_dsl_lpair_get(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("DslLpair", buffer);
        }

        if (nbd_dsl_trellis_get(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("DslTrellis", buffer);
        }

        if (nbd_dsl_snr_get(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("DslSnr", buffer);
        }

        if (nbd_dsl_bitswap_get(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("DslBitswap", buffer);
        }

        if (nbd_dsl_sesdrop_get(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("DslSesdrop", buffer);
        }

        if (nbd_dsl_sra_get(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("DslSra", buffer);
        }

        if(nbd_status_get("adsl_diag", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
                ewf_hdf_set("DslDiag", buffer);
	}

	return;
}

FORM_HANDLER
{
	int restart = 0;
	const char *mod_a = NULL, *mod_d = NULL, *mod_l = NULL, *mod_t = NULL,
		*mod_2 = NULL, *mod_p = NULL, *mod_e = NULL, *mod_m = NULL;

	mod_a = ewf_hdf_get("dsl_mod_a");
	mod_d = ewf_hdf_get("dsl_mod_d");
	mod_l = ewf_hdf_get("dsl_mod_l");
	mod_t = ewf_hdf_get("dsl_mod_t");
	mod_2 = ewf_hdf_get("dsl_mod_2");
	mod_p = ewf_hdf_get("dsl_mod_p");
	mod_e = ewf_hdf_get("dsl_mod_e");
	mod_m = ewf_hdf_get("dsl_mod_m");

	buffer[0] = '\0';
	strncat(buffer, mod_a ? mod_a : "", 1);
	strncat(buffer, mod_d ? mod_d : "", 1);
	strncat(buffer, mod_l ? mod_l : "", 1);
	strncat(buffer, mod_t ? mod_t : "", 1);
	strncat(buffer, mod_2 ? mod_2 : "", 1);
	strncat(buffer, mod_p ? mod_p : "", 1);
	strncat(buffer, mod_e ? mod_e : "", 1);
	strncat(buffer, mod_m ? mod_m : "", 1);

#ifndef X86
	if (nbd_dsl_mod_set(buffer) == NBD_SUCCESS)
	{
		restart++;
	}
#endif

	if (ewf_hdf_sget_copy("dsl_lpair",  buffer, sizeof  buffer) == 0)
	{
#ifndef X86
		if (nbd_dsl_lpair_set(buffer) == NBD_SUCCESS)
		{
			restart++;
		}
#endif
	}

	if (ewf_hdf_sget_copy("dsl_trellis",  buffer, sizeof  buffer) == 0)
	{
#ifndef X86
		if (nbd_dsl_trellis_set(buffer) == NBD_SUCCESS)
		{
			restart++;
		}
#endif
	}

	if (ewf_hdf_sget_copy("dsl_snr",  buffer, sizeof  buffer) == 0)
	{
#ifndef X86
		if (nbd_dsl_snr_set(buffer) == NBD_SUCCESS)
		{
			restart++;
		}
#endif
	}

	if (ewf_hdf_sget_copy("dsl_bitswap",  buffer, sizeof  buffer) == 0)
	{
#ifndef X86
		if (nbd_dsl_bitswap_set(buffer) == NBD_SUCCESS)
		{
			restart++;
		}
#endif
	}

	if (ewf_hdf_sget_copy("dsl_sesdrop",  buffer, sizeof  buffer) == 0)
	{
#ifndef X86
		if (nbd_dsl_sesdrop_set(buffer) == NBD_SUCCESS)
		{
			restart++;
		}
#endif
	}

	if (ewf_hdf_sget_copy("dsl_sra",  buffer, sizeof  buffer) == 0)
	{
#ifndef X86
		if (nbd_dsl_sra_set(buffer) == NBD_SUCCESS)
		{
			restart++;
		}
#endif
	}

	if (ewf_hdf_sget_copy("dsl_diag",  buffer, sizeof  buffer) == 0)
	{
                if (nbu_string_matches(buffer, "on") == NBU_STRING_EQUAL) {
                        nbd_dsl_diag_start();
                } else {
                        nbd_dsl_diag_stop();
                }
	}

	if (restart > 0)
	{
		nbd_user_config_set("adsl_profile", "0");
#ifndef X86
		nbd_dsl_restart();
#endif
	}

	return;
}
