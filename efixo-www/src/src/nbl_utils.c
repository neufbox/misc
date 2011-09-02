#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "nbl.h"
#include "nbl_utils.h"

#include "nbu/nbu.h"

#include "nbd.h"
#include "nbd/spy.h"

static char _nbl_utils_buf[256];

int nbl_utils_box_upgrading(void)
{
	int ret = -1;

#if defined(CIBOX)
        ret = nbd_status_get("autoconf_status", _nbl_utils_buf, sizeof(_nbl_utils_buf));
#else
	ret = nbd_autoconf_get("status", _nbl_utils_buf, sizeof(_nbl_utils_buf));
#endif
	if (ret == NBD_SUCCESS)
	{
		if(nbu_string_matches(_nbl_utils_buf, "downloading") == NBU_STRING_EQUAL
		   || nbu_string_matches(_nbl_utils_buf, "burning") == NBU_STRING_EQUAL)
		{
			ret = NBU_SUCCESS;
		}
		else
		{
			ret = NBU_ERROR;
		}
	}

	return ret;
}

/*
 * wait 2 seconds in a fork and call nbd_sys_reboot
 */
int nbl_utils_box_reboot(void)
{
	pid_t pid;
	int ret = NBU_SUCCESS;

	pid = fork();
	if(pid == 0)
	{
		/* child */
		sleep(2);

		nbd_connect();
		/* Send spy event that we are going for a reboot */
		nbd_spy_event("reboot");
		sleep(1);
		nbd_sys_reboot();

		nbd_disconnect();

		exit(EXIT_SUCCESS);
	}
	else if(pid == -1)
	{
		nbu_log_error("An error occur when trying to fork ! %s", strerror(errno));
		ret = NBU_ERROR;
	}

	return ret;
}

int nbl_utils_timestamp2ltime(int timestamp, nbl_utils_ltime_t* ltm)
{
	int ts = timestamp;

	if(ltm == NULL)
	{
		nbu_log_error("ltp is NULL !");
		return NBU_ERROR;
	}

	ltm->sec = ts % 60;

	ts = ts / 60;
	ltm->min = ts % 60;

	ts = ts / 60;
	ltm->hour = ts % 24;

	ts = ts / 24;
	ltm->day = ts;

	return NBU_SUCCESS;
}

int nbl_utils_strcut(const char *str,
                     char cr,
                     char *buf1, size_t buf1_size,
                     char *buf2, size_t buf2_size)
{
	unsigned int i;
        int icr = -1;

        i = 0;
	while ( str[i] != '\0' )
        {
                if(str[i] == cr)
                {
                        icr = i;
                }
                i++;
	}

        if(icr != -1)
        {
                nbu_string_printf(buf1, buf1_size,
                                  "%.*s",
                                  icr,
                                  str);
                nbu_string_copy(buf2, buf2_size,
                                str + icr + 1);
        }
        else
        {
                nbu_string_copy(buf1, buf1_size, str);
                nbu_string_copy(buf2, buf2_size, "\0");
        }

        return NBU_SUCCESS;
}
