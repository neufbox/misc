#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>

#include "webapp.h"

#include "nbl.h"
#include "nbd.h"

PAGE_DF("/rootfs", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        if(nbd_user_config_get("rootfs", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("rootfs_choose", buffer);
        }
        else
        {
                /* not found, flash by default */
                ewf_hdf_set("rootfs_choose", "flash");
        }

}

FORM_HANDLER
{
        if(ewf_hdf_sget_copy("rootfs",  buffer, sizeof  buffer) == NBD_SUCCESS)
        {
                if(nbd_sys_set_rootfs(buffer) == NBD_SUCCESS)
                {
                        nbd_user_config_commit();

                        ewf_utils_redirect(WEBAPP_URI_REBOOT);
                }
        }
}
