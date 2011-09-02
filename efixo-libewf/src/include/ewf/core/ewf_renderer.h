#ifndef _EWF_RENDERER_H_
#define _EWF_RENDERER_H_

#include "ewf/ewf_page.h"
#include "ewf/core/ewf_request.h"

#define CONTENT_TYPE_AUTO 0
#define CONTENT_TYPE_HTML 1
#define CONTENT_TYPE_XML 2

extern void ewf_renderer(ewf_request_t *request, ewf_page_t *page);

extern void ewf_renderer_force_raw_output(void);

extern void ewf_renderer_force_content_type_output(int type);

#endif
