#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbl_utils.h"

#include "nbd.h"

PAGE_DFA_u("/reboot", EWF_ACCESS_PRIVATE);

DISPLAY_AJAX_HANDLER
{
	return;
}

FORM_HANDLER
{
	ewf_hdf_set("IsRebooting", "true");

	nbl_utils_box_reboot();

	return;
}
