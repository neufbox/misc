#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

#include <ezxml.h>

PAGE_DF("/network/route", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        ezxml_t root, node;

        char *buf_xml = NULL;
        size_t buf_xml_size;
	char parts[4][4];
	int n;

	if (nbd_user_config_get("lan_ipaddr", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		nbu_net_split_ip(buffer, parts);
		ewf_hdf_set("route_gateway_p0", parts[0]);
		ewf_hdf_set("route_gateway_p1", parts[1]);
		ewf_hdf_set("route_gateway_p2", parts[2]);
	}

        if(nbd_user_config_xml("route", &buf_xml, &buf_xml_size)  == NBD_SUCCESS) {
                root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(root != NULL)
		{
                        n = 0;
			for(node = ezxml_child(root, "rule"); node; node = node->next)
			{
				++n;

				nbu_string_printf(buffer, sizeof buffer, "route_list.%d.destination", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "destination"));

				nbu_string_printf(buffer, sizeof buffer, "route_list.%d.gateway", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "gateway"));

				nbu_string_printf(buffer, sizeof buffer, "route_list.%d.genmask", n);
				ewf_hdf_set(buffer, ezxml_child_txt(node, "genmask"));
			}

			ezxml_free(root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }
                free(buf_xml);
        }

	return;
}

FORM_HANDLER
{
	const char *action = NULL;
	char name[32], dest_ip[16], dest_mask[16], gateway_ip[16];
	char parts[4][4];
	int restart = 0;
	int i;

	action = ewf_hdf_sget("action");

	/* user added a route */
	if (nbu_string_ncompare(action, sizeof("add") - 1, "add") == NBU_STRING_EQUAL)
	{
		/* route_destination */
		for (i = 0; i < 4; i++)
		{
			nbu_string_printf(name, sizeof name, "route_destination_p%d", i);

			if (ewf_hdf_sget_copy(name, buffer, sizeof  buffer) == EWF_SUCCESS)
			{
				nbu_string_copy(parts[i], sizeof parts[i], buffer);
			}
			else
			{
				parts[i][0] = '\0';
			}
		}

		if (nbu_net_join_ip(parts[0], parts[1], parts[2], parts[3], dest_ip, sizeof(dest_ip)) == 0)
		{
			restart++;
		}
		else
		{
			nbu_log_error("Unable to join ip '%s %s %s %s'",
				      parts[0], parts[1], parts[2], parts[3]);

			ewf_hdf_set_error("route_destination", "invalid_ip");
		}

		/* route_genmask */
		for (i = 0; i < 4; i++)
		{
			nbu_string_printf(name, sizeof name, "route_genmask_p%d", i);

			if (ewf_hdf_sget_copy(name,  buffer, sizeof  buffer) == 0)
			{
				nbu_string_copy(parts[i], sizeof parts[i], buffer);
			}
			else
			{
				parts[i][0] = '\0';
			}
		}

		if (nbu_net_join_ip(parts[0], parts[1], parts[2], parts[3], dest_mask, sizeof(dest_mask)) == 0)
		{
			if (nbu_net_validate_netmask(dest_mask) == 0)
			{
				restart++;
			}
			else
			{
				nbu_log_error("Invalid netmask '%s'", dest_mask);

				ewf_hdf_set_error("route_genmask", "invalid_netmask");
			}
		}
		else
		{
			nbu_log_error("Unable to join netmask ip '%s %s %s %s'",
				      parts[0], parts[1], parts[2], parts[3]);

			ewf_hdf_set_error("route_genmask", "invalid_netmask");
		}

		/* route_gateway */
		if (nbd_user_config_get("lan_ipaddr", buffer, sizeof buffer) == NBD_SUCCESS)
		{
			if (nbu_net_split_ip(buffer, parts) == 0)
			{
				if (ewf_hdf_sget_copy("route_gateway_p3",  buffer, sizeof  buffer) == 0)
				{
					nbu_string_copy(parts[3], sizeof parts[3], buffer);
				}
				else
				{
					parts[3][0] = '\0';
				}

				if (nbu_net_join_ip(parts[0], parts[1], parts[2], parts[3], gateway_ip, sizeof(gateway_ip)) == 0)
				{
					restart++;
				}
				else
				{
					ewf_hdf_set_error("route_gateway", "invalid_ip");
				}
			}
		}

		if(restart == 3)
		{
			/* data are ok ! */
			if(nbd_uroute_add(dest_ip, dest_mask, gateway_ip) != NBD_SUCCESS)
			{
				ewf_hdf_form_set_error("unknown");
			}
		}
	}
	/* user removed a route */
	else if (nbu_string_ncompare(action, sizeof("remove") - 1, "remove") == NBU_STRING_EQUAL)
	{
		if (nbd_uroute_del(ewf_hdf_sget_int("row")) != NBD_SUCCESS)
		{
			ewf_hdf_form_set_error("unknown");
		}
	}

	return;
}
