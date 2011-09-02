#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"
#include "nbd/lan.h"

#include <ezxml.h>

PAGE_DF("/network/dns", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        ezxml_t root, node;
        char *buf_xml = NULL;
        size_t buf_xml_size;
	int n;

#ifndef X86
        if(nbd_user_config_xml("dns", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(root != NULL)
		{
                        n = 0;
			for(node = ezxml_child(root, "host"); node; node = node->next)
			{
				++n;

				nbu_string_printf(buffer, sizeof buffer, "DnshostsList.%d.ip", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "ip"));

				nbu_string_printf(buffer, sizeof buffer, "DnshostsList.%d.hostname", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "domain"));
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
#ifndef X86
	const char* action = NULL;
	const char* dnshosts_ip_p0 = NULL, *dnshosts_ip_p1 = NULL,
                *dnshosts_ip_p2 = NULL, *dnshosts_ip_p3 = NULL,
                *dnshosts_hostname = NULL;
	int row, ret = NBD_ERROR;
	int restart = 0;

	char buf_ip[16];

        EWF_HDF_GET(action);

        if (nbu_string_matches(action, "add") == NBU_STRING_EQUAL)
        {
                EWF_HDF_GET(dnshosts_ip_p0);
                EWF_HDF_GET(dnshosts_ip_p1);
                EWF_HDF_GET(dnshosts_ip_p2);
                EWF_HDF_GET(dnshosts_ip_p3);
                EWF_HDF_GET(dnshosts_hostname);

                if(nbu_net_join_ip(dnshosts_ip_p0,
                                   dnshosts_ip_p1,
                                   dnshosts_ip_p2,
                                   dnshosts_ip_p3,
                                   buf_ip,
                                   sizeof(buf_ip)) == NBU_SUCCESS
                   && nbu_net_validate_ip(buf_ip) == NBU_SUCCESS)
                {
                        ret = nbd_dnshosts_add(buf_ip,
                                               dnshosts_hostname);

                        if(ret == NBD_SUCCESS)
                        {
                                restart++;
                        }
                        else
                        {
                                /* an error occured */
                                nbu_log_error("nbd_dnshosts_add(%s, %s) "
                                              "failed !",
                                              buf_ip,
                                              dnshosts_hostname);

                                if(ret == lan_error_dnshost_already_exist)
                                {
                                        ewf_hdf_form_set_error("duplicate");
                                }
                                else if(ret == lan_error_dnshost_hostname_already_used)
                                {
                                        ewf_hdf_form_set_error("hostname_already_used");
                                }
                                else
                                {
                                        ewf_hdf_form_set_error("unknown");
                                }
                        }
                }
                else
                {
                        ewf_hdf_set_error("dnshosts_ip", "invalid");
                }
        }
        else
        {
                /* user want to remove an item */
                EWF_HDF_GET_INT(row);

                if((ret = nbd_dnshosts_del(row)) == NBD_SUCCESS)
                {
                        restart++;
                }
                else
                {
                        /* an error occured */
                        nbu_log_error("nbd_dnshosts_del(%d) failed (ret=%d) !",
                                      row, ret);

                        ewf_hdf_form_set_error("unknown");
                }
        }

        if(restart > 0)
        {
                nbd_user_config_commit();

                nbd_sys_sync_run(DHCPD_START_STOP,
                                 "restart");
	}
#endif

	return;
}
