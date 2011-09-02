#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

PAGE_D("/segfault", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	char *c = NULL;

	c[2] = 't';

	return;
}
