#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbd.h"

PAGE_DFA_u("/service/printer", EWF_ACCESS_PRIVATE);

DISPLAY_AJAX_HANDLER
{
	if (nbd_status_get("p910nd_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("PrinterSharingStatus", buffer);
	}

	if (nbd_user_config_get("p910nd_bidir", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("PrinterSharingBidir", buffer);
	}

	return;
}

FORM_HANDLER
{
        const char *printer_sharing_bidir = NULL;
        struct trx trx;

        EWF_HDF_GET(printer_sharing_bidir);

        NV_TRX_INIT(trx);

        NV_SET_WTRX(trx, "p910nd_bidir", printer_sharing_bidir);

        if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
        {
		nbd_sys_async_run(P910D_START_STOP,
                                  "restart");
        }

	return;
}
