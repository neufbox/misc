#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbl_utils.h"

#include "nbd.h"

#include <ezxml.h>

PAGE_D("/test", EWF_ACCESS_PUBLIC);

DISPLAY_HANDLER
{
	//ezxml_t xmln_root, xmln_port;

	//char *buf_xml = NULL;
	//size_t buf_xml_size;

	int n;

	/*
	 * for JS::basename
	 */
	const char *a_urls[] = {
		"http://192.168.9.1/login?page_ref=/state",
		"http://192.168.9.1/state",
		"http://192.168.9.1/bidule/truc",
		"http://192.168.9.1/",
		"http://",
		"http:",
		NULL,
	};

	n = 0;
	while(a_urls[n] != NULL)
	{
		nbu_string_printf(buffer, sizeof buffer, "urls.%d.value", n);
		ewf_hdf_set(buffer, a_urls[n]);

		n++;
	}

	return;
}
