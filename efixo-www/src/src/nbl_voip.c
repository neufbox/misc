#include "nbl.h"
#include "nbl_voip.h"

#include "nbd.h"

#include "nbu/nbu.h"
#include "nbu/string.h"

#include "nbd/nvram.h"
#include "nbd/status.h"

int nbl_voip_get_hook_status(char *status, size_t status_size)
{
    char buf[32];
	int ret = NBD_ERROR;

	ret = nbd_status_get("voip_up", buf, sizeof(buf));
	if (ret == NBD_ERROR)
	{
		return NBU_ERROR;
	}

	if (nbu_string_matches(buf, "up") == NBU_STRING_EQUAL)
	{
		ret = nbd_status_get("voip_hookstate", buf, sizeof(buf));
		if (ret == NBD_SUCCESS)
		{
			if (nbu_string_matches(buf, "onhook") == NBU_SUCCESS)
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
