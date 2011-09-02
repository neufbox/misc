#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>

#include <nbd/config.h>

#include "nbl.h"
#include "nbl_adsl.h"
#include "nbl_net.h"

#include "nbd.h"

PAGE_D("/state/wan/extra", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
#ifdef NB5
	if(nbd_status_get("link_wan0_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("sfp_status", buffer);

		if(nbu_string_compare(buffer, "up") == 0)
		{
			/* get more informations */
			if(nbd_sfp_get("serial_base_wavelength", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_wavelength", buffer);
			}

			if(nbd_sfp_get("serial_base_vendor-name", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_vendor_name", buffer);
			}

			if(nbd_sfp_get("serial_base_vendor-PN", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_vendor_pn", buffer);
			}

			if(nbd_sfp_get("serial_base_vendor-rev", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_vendor_rev", buffer);
			}

			if(nbd_sfp_get("serial_extended_serial-number", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_extended_serial_number", buffer);
			}

			if(nbd_status_get("link_wan0_speed", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_speed", buffer);
			}

			if(nbd_sfp_get("monitoring_temperature_value", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_monitoring_temperature", buffer);
			}

			if(nbd_sfp_get("monitoring_vcc_value", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_monitoring_vcc", buffer);
			}

			if(nbd_sfp_get("monitoring_bias_value", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_monitoring_bias", buffer);
			}

			if(nbd_sfp_get("monitoring_tx-power_value", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_monitoring_tx_power", buffer);
			}

			if(nbd_sfp_get("monitoring_rx-power_value", buffer, sizeof(buffer)) == NBD_SUCCESS)
			{
				ewf_hdf_set("sfp_monitoring_rx_power", buffer);
			}
		}
		else
		{
                        ewf_utils_redirect(URI_PAGE_INDEX);
		}
	}

#else
	char buf[512];
	int i = 0;
	int8_t volt;
        char infra[32];

	if(nbd_status_get("adsl_link", infra, sizeof(infra)) != NBU_SUCCESS)
        {
                infra[0] = '\0';
        }

        if(has_net_infra_adsl()
           && nbu_string_matches(infra, "up") == NBU_STRING_EQUAL)
        {
		nbu_log_debug("get info about adsl connection");

		/* ADSL Stats */
		i = 0;
	        while (nbl_adsl_get_stats(buf, sizeof buf) >= 0)
		{
			nbu_string_printf(buffer, sizeof buffer, "ADSLStats.part%d", i);
			ewf_hdf_set(buffer, buf);

			++i;
		}

		/* ADSL Diag */
		i = 0;
		while (nbl_adsl_get_linediag(buf, sizeof buf) >= 0)
		{
			nbu_string_printf(buffer, sizeof buffer, "ADSLLineDiag.part%d", i);
			ewf_hdf_set(buffer, buf);

			++i;
		}

                /* ADSL Stats bits */
		while (nbl_adsl_get_stats_bits(buf, sizeof buf) >= 0)
		{
			nbu_string_printf(buffer, sizeof buffer, "ADSLLineDiag.part%d", i);
			ewf_hdf_set(buffer, buf);

			++i;
		}


		/* Line voltage */
#ifndef X86
		if(nbd_voip_get_line_voltage(&volt) == NBD_SUCCESS)
#else
		volt = 3;
#endif
		{
			ewf_hdf_set_int("LineVoltage", volt);
		}
	}
	else
	{
		ewf_utils_redirect(URI_PAGE_INDEX);
	}
#endif

	return;
}
