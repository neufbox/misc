#include "nbd.h"
#include <nbd/config.h>
#include "nbd/status.h"

#include "nbl_trash.h"

#include "nbu/nbu.h"

int nbl_voip_get_hook_status(char *status, size_t status_size)
{
	char voip_up[sizeof("down")];
	int ret = NBD_ERROR;

	ret = nbd_status_get("voip_up", voip_up, sizeof(voip_up));
	if (ret == NBD_ERROR)
	{
		return NBU_ERROR;
	}

	if (nbu_string_ncompare(status, sizeof("up") - 1, "up") == NBU_STRING_EQUAL)
	{
		ret = nbd_status_get("voip_hookstate", status, status_size);
		if (ret == NBD_SUCCESS)
		{
			if (nbu_string_ncompare(status, sizeof("onhook") - 1, "onhook") == NBU_SUCCESS)
			{
				nbu_string_printf(status, status_size, "onhook");
			}
			else
			{
				nbu_string_printf(status, status_size, "offhook");
			}
		}
	}
	else
	{
		nbu_string_printf(status, status_size, "unknown");
	}

	return (ret == NBD_SUCCESS ? NBU_SUCCESS : NBU_ERROR);
}
