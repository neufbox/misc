#include "api_common.h"

#include "nbl_trash.h"

#include "nbd.h"

#include <ezxml.h>


static int voip_call_sort_func(void *a, void *b)
{
        /* most recent to most ancient */
        return (((struct nbd_phone_call_t*)b)->date - ((struct nbd_phone_call_t*)a)->date);
}

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

API_REGISTER_METHOD(voip, getInfo, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE)
{
        if (nbd_status_get("voip_up", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                api_set_value("voip.status", buffer);
        }

        if (nbd_status_get("net_voip_access", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                
                api_set_value("voip.infra", buffer);
        }

        if (nbl_voip_get_hook_status(buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                api_set_value("voip.hookstatus", buffer);
        }

        if(nbd_nv_get("voip_callhistory", buffer, sizeof buffer) == NBD_SUCCESS)
        {

                api_set_value("voip.callhistory",buffer);
        }
}

API_REGISTER_METHOD(voip, getCallhistoryList, EWF_HTTP_REQUEST_METHOD_GET, API_ACCESS_POLICY_PRIVATE)
{
        ezxml_t root, node;
        char *buf_xml = NULL;
        size_t buf_xml_size;
	int n;
        char buf2[128];

        nbu_log_debug("voip.getCallhistoryList Go go go...");
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
				api_set_value(buffer, voip_call_type_str(type));

				/* rule direction */
				sscanf(ezxml_child_txt(node, "direction"), "%hu", &direction);
				nbu_string_printf(buffer, sizeof buffer, "call_list.%d.direction", n);
				api_set_value(buffer, direction == 0 ? "incoming" : "outgoing");

				/* rule number */
				number = ezxml_child_txt(node, "number");
				nbu_string_printf(buffer, sizeof buffer, "call_list.%d.number", n);
				if(strlen(number) > 4)
				{
					nbu_string_printf(buf2, sizeof buf2, "%.*s XXXX", strlen(number) - 4, number);
					api_set_value(buffer, buf2);
				}
				else
				{
					api_set_value(buffer, number);
				}

				/* rule length */
				nbu_string_printf(buffer, sizeof buffer, "call_list.%d.length", n);
				api_set_value(buffer, ezxml_child_txt(node, "length"));

				/* rule date */
				nbu_string_printf(buffer, sizeof buffer, "call_list.%d.date", n);
				api_set_value(buffer, ezxml_child_txt(node, "date"));

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
}

API_REGISTER_METHOD(voip, start, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
        nbd_voip_start( );
}

API_REGISTER_METHOD(voip, stop, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
        nbd_voip_stop( );
}

API_REGISTER_METHOD(voip, restart, EWF_HTTP_REQUEST_METHOD_POST, API_ACCESS_POLICY_PRIVATE)
{
        nbd_voip_restart( );
}
