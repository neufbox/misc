#include "nbl.h"
#include "nbl_tv.h"

#include "nbd.h"
#include "nbd/status.h"

#include "nbu/nbu.h"

int nbl_tv_get_status(char *buf, size_t buf_size)
{
	int ret = NBU_ERROR;
	int count;

	count = nbd_stb_list_count();
	nbu_string_printf(buf, buf_size, "%s", count == 0 ? "down" : "up");
	ret = NBU_SUCCESS;

	return ret;
}

