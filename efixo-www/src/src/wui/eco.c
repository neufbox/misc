#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

PAGE_DA_u("/eco", EWF_ACCESS_PRIVATE);

DISPLAY_AJAX_HANDLER
{
        status_to_hdf("eco_status", "eco_status");
        nv_to_hdf("eco_profile", "eco_profile");
        nv_to_hdf("eco_voip_active", "eco_voip_active");
        nv_to_hdf("eco_cpl_active", "eco_cpl_active");
}
