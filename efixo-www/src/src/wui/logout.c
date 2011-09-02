#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>
#include <ewf/core/ewf_smith.h>
#include <ewf/core/ewf_config.h>

#include "nbl.h"
#include "nbd.h"

PAGE_D("/logout", EWF_ACCESS_PUBLIC);

DISPLAY_HANDLER
{
	ewf_smith_logout();

	ewf_utils_redirect(URI_PAGE_INDEX);
}
