#include "ewf/ewf.h"
#include "ewf/ewf_page.h"

#include "ewf/core/ewf_dispatcher.h"

/*************************/
/* public fcts           */
/*************************/

void ewf_page_register(ewf_page_t *page)
{
	ewf_dispatcher_registerpage(page);
}
