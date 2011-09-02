#include <time.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include <nbu/list.h>

#include "nbl.h"
#include "nbl_voip.h"
#include "nbl_net.h"
#if defined(NB4) || defined(NB5)
#include "nbl_leds.h"
#endif

#include "nbd.h"
#include "dbl.h"

#include <ezxml.h>

PAGE_DFA("/state/voip", EWF_ACCESS_PRIVATE);

static const char *voip_call_type_str(unsigned short type)
{
        static struct {
                unsigned short type;
                char *str;
        } voip_call_type_num2str[] = {
                { NBD_PHONE_CALL_PSTN, "pstn", },
                { NBD_PHONE_CALL_VOIP, "voip", },
                { NBD_PHONE_CALL_COMPLEX, "voip", }, /* COMPLEX is complex.*/
                { NBD_PHONE_CALL_3G, "radio", },
        };
        unsigned int i;

        for(i = 0; i < NBU_ARRAY_SIZE(voip_call_type_num2str); i++)
        {
                if(type == voip_call_type_num2str[i].type)
                {
                        return voip_call_type_num2str[i].str;
                }
        }

        return "unknown";
}

DISPLAY_HANDLER
{
        ezxml_t root, node;
        char *buf_xml = NULL;
        size_t buf_xml_size;
	int n;
        char buf2[128];
        struct tm t;
#if defined(NB4) || defined(NB5)
        const char *led_brightness = nbl_leds_brightness_cstr();
#endif /* defined(NB4) || defined(NB5) */

#if defined(NB4) || defined(NB5)
        ewf_hdf_set("leds_state", led_brightness);
#endif /* defined(NB4) || defined(NB5) */

        if (nbd_status_get("voip_up", buffer, sizeof(buffer)) == NBU_SUCCESS)
        {
                ewf_hdf_set("voip_linestatus", buffer);
        }

        if (nbl_voip_get_hook_status(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("voip_hookstatus", buffer);
        }

        if(nbd_status_get("net_voip_access", buffer, sizeof(buffer)) == NBU_SUCCESS)
        {
                ewf_hdf_set("voip_access", buffer);
        }

#ifndef NB5
        status_to_hdf("voip_daastatus", "voip_daastatus");
        status_to_hdf("voip_slicstatus", "voip_slicstatus");
        status_to_hdf("voip_slicerrnum", "voip_slicerrnum");
#endif

#ifndef CIBOX
        nv_to_hdf("voip_usermissedcall", "voip_usermissedcall");
#endif

#ifndef X86
#ifdef NB6
        nv_to_hdf("eco_led_active", "eco_led_active");
#endif
        nv_to_hdf("voip_callhistory", "CallHistoryActive");

        if(nbd_status_xml("voip_history", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(root != NULL)
		{
                        n = 0;

			for(node = ezxml_child(root, "call"); node; node = node->next)
			{
				unsigned short type;    /* NBD_PHONE_CALL_PSTN or NBD_PHONE_CALL_VOIP */
				unsigned short direction;       /* NBD_PHONE_CALL_INCOMING or NBD_PHONE_CALL_OUTGOING */
				char const *number;        /* phone number of the interlocutor */
				time_t date;            /* date of the call */

				/* rule type */
				sscanf(ezxml_child_txt(node, "type"), "%hu", &type);
				nbu_string_printf(buffer, sizeof buffer, "call_list.%d.type", n);
				ewf_hdf_set(buffer, voip_call_type_str(type));

				/* rule direction */
				sscanf(ezxml_child_txt(node, "direction"), "%hu", &direction);
				nbu_string_printf(buffer, sizeof buffer, "call_list.%d.direction", n);
				ewf_hdf_set(buffer, direction == 0 ? "incoming" : "outgoing");

				/* rule number */
				number = ezxml_child_txt(node, "number");
				nbu_string_printf(buffer, sizeof buffer, "call_list.%d.number", n);
				if(strlen(number) > 4)
				{
					nbu_string_printf(buf2, sizeof buf2, "%.*s XXXX", strlen(number) - 4, number);
					ewf_hdf_set(buffer, buf2);
				}
				else
				{
					ewf_hdf_set(buffer, number);
				}

				/* rule length */
				nbu_string_printf(buffer, sizeof buffer, "call_list.%d.length", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "length"));

				/* rule date */
				sscanf(ezxml_child_txt(node, "date"), "%ld", &date);
				if(localtime_r(&date, &t) != NULL)
				{
					nbu_string_printf(buffer, sizeof buffer, "call_list.%d.date", n);
					strftime(buf2, sizeof buf2, "%A %d %B %Y, %X", &t);
					ewf_hdf_set(buffer, buf2);

					nbu_string_printf(buffer, sizeof buffer, "call_list.%d.date.weekday", n);
					nbu_string_printf(buf2, sizeof buf2, "%d", t.tm_wday);
					ewf_hdf_set(buffer, buf2);

					nbu_string_printf(buffer, sizeof buffer, "call_list.%d.date.monthday", n);
					nbu_string_printf(buf2, sizeof buf2, "%d", t.tm_mday);
					ewf_hdf_set(buffer, buf2);

					nbu_string_printf(buffer, sizeof buffer, "call_list.%d.date.month", n);
					nbu_string_printf(buf2, sizeof buf2, "%d", t.tm_mon);
					ewf_hdf_set(buffer, buf2);

					nbu_string_printf(buffer, sizeof buffer, "call_list.%d.date.year", n);
					nbu_string_printf(buf2, sizeof buf2, "%d", t.tm_year + 1900);
					ewf_hdf_set(buffer, buf2);

					nbu_string_printf(buffer, sizeof buffer, "call_list.%d.date.hour", n);
					strftime(buf2, sizeof buf2, "%Hh%M", &t);
					ewf_hdf_set(buffer, buf2);
				}

				n++;
			}

			ezxml_free(root);
		}
		else
		{
			nbu_log_error("Unable to parse xml string");
		}
		free(buf_xml);
	}
#endif

        return;
}

FORM_HANDLER
{
        const char* action = NULL,
                *led_notification_active = NULL,
                *callhistory_active = NULL;
        struct trx trx;

        EWF_HDF_GET(action);

        if(nbu_string_matches(action, "cleanup") == NBU_STRING_EQUAL)
        {
#ifndef X86
                /* Cleanup call history */
                nbd_voip_callhistory_flush();
#endif
        }
        else if(nbu_string_matches(action, "led_notification") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(led_notification_active);

                NV_TRX_INIT(trx);

                NV_SET_WTRX(trx,
                            "voip_usermissedcall", led_notification_active);

                if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
                {
#ifndef X86
                        nbd_voip_manage_missed_calls();
#endif
                }
        }
        else if(nbu_string_matches(action, "conf") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(callhistory_active);

                NV_TRX_INIT(trx);

                NV_SET_WTRX(trx,
                            "voip_callhistory", callhistory_active);

                if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
                {
#ifndef X86
                        nbd_voip_callhistory_flush();
#endif
                }
        }
}

AJAX_HANDLER
{
        if(nbd_status_get("voip_up", buffer, sizeof(buffer)) == NBU_SUCCESS)
        {
                ewf_hdf_set("voip_linestatus", buffer);
        }

        if (nbl_voip_get_hook_status(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("voip_hookstatus", buffer);
        }

        if(nbd_status_get("net_voip_access", buffer, sizeof(buffer)) == NBU_SUCCESS)
        {
                ewf_hdf_set("voip_access", buffer);
        }

        return;
}
