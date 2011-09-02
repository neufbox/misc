#include "dbl.h"

#include <ewf/ewf.h>

#include "nbd.h"

int nv_to_hdf(const char const *nvram_name, const char const *hdf_name)
{
        char buf[256];
        int ret = EWF_ERROR;

	if(nbd_user_config_get(nvram_name, buf, sizeof buf) == NBD_SUCCESS)
	{
		ewf_hdf_set(hdf_name, buf);
		ret = EWF_SUCCESS;
	}
	else
	{
		nbu_log_error("Unable to get nvram '%s' value",
                              nvram_name);
	}

	return ret;
}

int status_to_hdf(const char const *status_name, const char const *hdf_name)
{
        char buf[256];
        int ret = EWF_ERROR;

	if(nbd_status_get(status_name, buf, sizeof buf) == NBD_SUCCESS)
	{
		ewf_hdf_set(hdf_name, buf);
		ret = EWF_SUCCESS;
	}
	else
	{
		nbu_log_error("Unable to get status '%s' value",
                              status_name);
	}

	return ret;
}

int hdf_to_nv(const char *hdf_name, const char *nvram_name)
{
        const char *value = NULL;
        int ret = NV_SET_ERROR;

	if((value = ewf_hdf_sget(hdf_name)) != NULL)
	{
		ret = nbd_user_config_set(nvram_name, value);
	}
	else
	{
		nbu_log_error("Unable to get secure hdf '%s' value",
                              hdf_name);
	}

        return ret;
}
