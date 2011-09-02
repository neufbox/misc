#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

PAGE_D("/state/wifi", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
#ifndef X86
	char *buf;
	size_t outlen;

	if(nbd_wlan_get_counters(&buf, &outlen) == NBD_SUCCESS)
	{
		ewf_hdf_set("rawstats.wifi", buf);

		free(buf);
	}
#endif

	return;
}
