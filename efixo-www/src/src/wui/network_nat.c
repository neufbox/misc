#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

#include <ezxml.h>

PAGE_DF("/network/nat", EWF_ACCESS_PRIVATE);

#define RULENAME_SIZE   16
#define PROTO_SIZE      8
#define EXTPORT_SIZE    16
#define DSTIP_SIZE      16
#define DSTPORT_SIZE    16
#define NAT_ACTIVATED_SIZE  4
struct nat_s {
       char rulename[RULENAME_SIZE];
       char proto[PROTO_SIZE];
       char ext_port[EXTPORT_SIZE];
       char dst_ip[DSTIP_SIZE];
       char dst_port[DSTPORT_SIZE];
       char activated[NAT_ACTIVATED_SIZE];
};

DISPLAY_HANDLER
{
#ifndef X86
        char ip_parts[4][4];
        unsigned int n;

        char *buf_xml;
        size_t buf_xml_size;
        ezxml_t xmln_root, xmln_rule;

        if (nbd_user_config_get("lan_ipaddr", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                nbu_net_split_ip(buffer, ip_parts);

                ewf_hdf_set("DstIPPart1", ip_parts[0]);
                ewf_hdf_set("DstIPPart2", ip_parts[1]);
                ewf_hdf_set("DstIPPart3", ip_parts[2]);

                ewf_hdf_set("ListDstIPPart1", ip_parts[0]);
                ewf_hdf_set("ListDstIPPart2", ip_parts[1]);
                ewf_hdf_set("ListDstIPPart3", ip_parts[2]);

                ewf_hdf_set("dmz_ip0", ip_parts[0]);
                ewf_hdf_set("dmz_ip1", ip_parts[1]);
                ewf_hdf_set("dmz_ip2", ip_parts[2]);
        }

        if( nbd_user_config_xml("nat", &buf_xml, &buf_xml_size) == NBD_SUCCESS )
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        n = 0;
                        for(xmln_rule = ezxml_child(xmln_root, "rule"); xmln_rule; xmln_rule = xmln_rule->next)
                        {
                                ++n;

                                nbu_string_printf(buffer, sizeof buffer, "List.%d.RuleName", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_rule, "rulename")));

                                nbu_string_printf(buffer, sizeof buffer, "List.%d.Proto", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_rule, "proto")));

                                nbu_string_printf(buffer, sizeof buffer, "List.%d.ExtPort", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_rule, "extport")));

                                nbu_string_printf(buffer, sizeof buffer, "List.%d.DstIP", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_rule, "dstip")));

                                nbu_string_printf(buffer, sizeof buffer, "List.%d.DstPort", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_rule, "dstport")));

                                nbu_string_printf(buffer, sizeof buffer, "List.%d.Activated", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_rule, "activated")));
                        }

                        ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }

                free(buf_xml);
        }

        if(nbd_user_config_get("upnpd_active", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("upnpd_active", buffer);
        }

        ewf_hdf_set_int("upnpd_rules_count", nbd_nat_upnp_count());

        if(nbd_user_config_get("dmz_active", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("dmz_active", buffer);
        }

        if(nbd_user_config_get("dmz_ipaddr", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                //set to 0
                memset(&ip_parts, 0, sizeof(ip_parts));
                nbu_net_split_ip(buffer, ip_parts);
                ewf_hdf_set("dmz_ip3", ip_parts[3]);
        }
#endif

        return;
}

FORM_HANDLER
{
#ifndef X86
        const char *action = NULL;

        int restart, number, ret, range = 0;
        struct nat_s rule;

        struct ewf_hdf_iterator ctx_var;

        char *xml;
        size_t xml_len;
        ezxml_t node, child;

        const char *ip0 = NULL, *ip1 = NULL, *ip2 = NULL, *ip3 = NULL,
                *nat_rulename = NULL,
                *nat_proto = NULL,
                *nat_extport = NULL,
                *nat_extrange_p0 = NULL,
                *nat_extrange_p1 = NULL,
                *nat_dstport = NULL,
                *nat_dstrange_p0 = NULL,
                *nat_dstrange_p1 = NULL,
                *nat_active = NULL;

        /* Set to 0 */
        memset(&rule, 0, sizeof(rule));

        action = ewf_hdf_sget("action");

        /* user added a rule */
        if (ewf_hdf_exist("Query.action_add"))
        {
                restart = 0;

                /* rulename */
                if((nat_rulename = ewf_hdf_sget("nat_rulename")) != NULL)
                {
                        nbu_string_printf(rule.rulename, RULENAME_SIZE, nat_rulename);
                        restart++;
                }

                /* proto */
                if((nat_proto = ewf_hdf_sget("nat_proto")) != NULL)
                {
                        nbu_string_printf(rule.proto, PROTO_SIZE, nat_proto);
                        restart++;
                }

                /** range */
                if(ewf_hdf_sget_copy("nat_range",  buffer, sizeof  buffer) == 0)
                {
                        if (nbu_string_compare(buffer, "true") == NBU_SUCCESS)
                        {
                                range = 1;
                        }
                        else
                        {
                                range = 0;
                        }
                }

                /* extport */
                if(range == 0)
                {
                        if((nat_extport = ewf_hdf_sget("nat_extport")) != NULL)
                        {
                                nbu_string_printf(rule.ext_port, EXTPORT_SIZE, nat_extport);
                                restart++;
                        }
                }
                else if(range == 1)
                {
                        if((nat_extrange_p0 = ewf_hdf_sget("nat_extrange_p0")) != NULL
                           && (nat_extrange_p1 = ewf_hdf_sget("nat_extrange_p1")) != NULL)
                        {
                                nbu_string_printf(rule.ext_port, EXTPORT_SIZE, "%s-%s", nat_extrange_p0, nat_extrange_p1);
                                restart++;
                        }
                }

                /* dstip */
                ip0 = ewf_hdf_sget("nat_dstip_p0");
                ip1 = ewf_hdf_sget("nat_dstip_p1");
                ip2 = ewf_hdf_sget("nat_dstip_p2");
                ip3 = ewf_hdf_sget("nat_dstip_p3");

                if(nbu_net_join_ip(ip0, ip1, ip2, ip3, buffer, sizeof(buffer)) == 0 && nbu_net_validate_ip(buffer) == 0)
                {
                        nbu_string_printf(rule.dst_ip, DSTIP_SIZE, buffer);
                        restart++;
                }
                else
                {
                        ewf_hdf_set_error("nat_dstip", "invalid_ip");
                }

                /* dstport */
                if(range == 0)
                {
                        if((nat_dstport = ewf_hdf_sget("nat_dstport")) != NULL)
                        {
                                nbu_string_printf(rule.dst_port, DSTPORT_SIZE, nat_dstport);
                                restart++;
                        }
                }
                else if (range == 1)
                {
                        if((nat_dstrange_p0 = ewf_hdf_sget("nat_dstrange_p0")) != NULL
                           && (nat_dstrange_p1 = ewf_hdf_sget("nat_dstrange_p1")) != NULL)
                        {
                                nbu_string_printf(rule.dst_port, DSTPORT_SIZE, "%s-%s", nat_dstrange_p0, nat_dstrange_p1);
                                restart++;
                        }
                }

                /* activated */
                if((nat_active = ewf_hdf_sget("nat_active")) == NULL)
                {
                        nat_active = "off";
                }
                nbu_string_printf(rule.activated, NAT_ACTIVATED_SIZE, nat_active);

                if(restart == 5)
                {
                        /* All informations is ok, let's start play with nbd */
                        if((ret = nbd_nat_add(rule.rulename, rule.proto, rule.ext_port, rule.dst_ip, rule.dst_port, rule.activated)) == NBD_SUCCESS)
                        {
                                nbd_user_config_commit();
                        }
                        else
                        {
                                if(ret == nat_error_invalid_protocol)
                                {
                                        ewf_hdf_set_error("nat_proto", "invalid_protocol");
                                }
                                else if(ret == nat_error_invalid_ip)
                                {
                                        ewf_hdf_set_error("nat_dstip", "invalid_ip");
                                }
                                else if(ret == nat_error_invalid_dstport)
                                {
                                        if(range == 0)
                                        {
                                                ewf_hdf_set_error("nat_dstport", "invalid_dstport");
                                        }
                                        else if(range == 1)
                                        {
                                                ewf_hdf_set_error("nat_dstrange", "invalid_dstportrange");
                                        }
                                }
                                else if(ret == nat_error_invalid_extport)
                                {
                                        if(range == 0)
                                        {
                                                ewf_hdf_set_error("nat_extport", "invalid_extport");
                                        }
                                        else if(range == 1)
                                        {
                                                ewf_hdf_set_error("nat_extrange", "invalid_extportrange");
                                        }
                                }
                                else if(ret == nat_error_extport_already_use)
                                {
                                        if(range == 0)
                                        {
                                                ewf_hdf_set_error("nat_extport", "extport_already_use");
                                        }
                                        else if(range == 1)
                                        {
                                                ewf_hdf_set_error("nat_extrange", "extport_already_use");
                                        }
                                }
                                else if(ret == nat_error_extport_reserved)
                                {
                                        if(range == 0)
                                        {
                                                ewf_hdf_set_error("nat_extport", "extport_reserved");
                                        }
                                        else if(range == 1)
                                        {
                                                ewf_hdf_set_error("nat_extrange", "extport_reserved");
                                        }

                                        /* Get port list reserved */
                                        if(nbd_system_xml_privateport(&xml, &xml_len) == NBD_SUCCESS)
                                        {
                                                if(!(node = ezxml_parse_str(xml, xml_len)))
                                                {
                                                        nbu_log_error("XML parse error after 'nbd_system_xml_privateport' !");
                                                }
                                                else
                                                {
                                                        number = 0;
                                                        for ( child = ezxml_child( node, "cmn" ); child; child = child->next )
                                                        {
                                                                nbu_string_printf(buffer, sizeof(buffer),
                                                                                  "PrivatePortList.%d.Port", number + 1);
                                                                ewf_hdf_set(buffer, child->txt);

                                                                number ++;
                                                        }

                                                        ezxml_free(node);
                                                }

                                                free(xml);
                                        }
                                }
                                else
                                {
                                        ewf_hdf_form_set_error("unknown");
                                }
                        }
                }
                else
                {
                        nbu_log_debug("nat rule insertion failed ! restart value = %i", restart);

                        ewf_hdf_form_set_error("unknown");
                }

        }
        else if (ewf_hdf_exist("Query.action_remove"))
        {
                EWF_HDF_FOREACH("Query.action_remove", &ctx_var);

                number = atoi(ctx_var.name);
                nbu_log_debug("action_remove=%i", number);
                if(nbd_nat_del_by_index(number) == NBD_SUCCESS)
                {
                        nbd_user_config_commit();
                }
        }
        else if (ewf_hdf_exist("Query.action_disable"))
        {
                EWF_HDF_FOREACH("Query.action_disable", &ctx_var);

                number = atoi(ctx_var.name);
                if(number != -1)
                {
                        nat_active = "off";
                        nbu_log_debug("%d - %s", number, nat_active);
                        if(nbd_nat_active( number, "off" ) == NBD_SUCCESS)
                        {
                                nbd_user_config_commit();
                        }
                        else
                        {
                                nbu_log_error("nbd_nat_active(%d, %s) failed !",  number, nat_active);
                        }
                }
        }
        else if (ewf_hdf_exist("Query.action_enable"))
        {
                EWF_HDF_FOREACH("Query.action_enable", &ctx_var);
                number = atoi(ctx_var.name);
                if(number != -1)
                {
                        nat_active = "on";

                        nbu_log_debug("%d - %s", number, nat_active);

                        if(nbd_nat_active( number, nat_active ) == NBD_SUCCESS)
                        {
                                nbd_user_config_commit();
                        }
                        else
                        {
                                nbu_log_error("nbd_nat_active(%d, %s) failed !",  number, nat_active);
                        }
                }
        }
        else if (action != NULL && nbu_string_compare(action, "upnpd_submit") == NBU_SUCCESS)
        {
                if(ewf_hdf_sget_copy("upnpd_active",
                                     buffer, sizeof  buffer) == 0)
                {
                        if (nbd_user_config_set("upnpd_active", buffer) == NBD_SUCCESS)
                        {
                                nbd_user_config_commit();

                                nbd_sys_async_run(UPNPIGD_START_STOP,
                                                  "restart");
                        }
                }
        }
        else if (action != NULL && nbu_string_compare(action, "dmz_submit") == NBU_SUCCESS)
        {
                restart = 0;

                ip0 = ewf_hdf_sget("dmz_ip0");
                ip1 = ewf_hdf_sget("dmz_ip1");
                ip2 = ewf_hdf_sget("dmz_ip2");
                ip3 = ewf_hdf_sget("dmz_ip3");
                if(nbu_net_join_ip(ip0, ip1, ip2, ip3,
                                   buffer, sizeof(buffer)) == 0
                   && nbu_net_validate_ip(buffer) == 0)
                {
                        nbu_log_debug("dmz_ipaddr %s", buffer);
                        if(nbd_user_config_set("dmz_ipaddr", buffer) == NBD_SUCCESS)
                        {
                                nbu_log_debug("set dmz_ipaddr %s", buffer);
                                restart++;
                        }
                }
                else
                {
                        ewf_hdf_set_error("dmz_ip3", "invalid");
                }

                if(ewf_hdf_sget_copy("dmz_active", buffer, sizeof(buffer)) == 0)
                {
                        if(nbd_user_config_set("dmz_active", buffer) == NBD_SUCCESS)
                        {
                                nbu_log_debug("set dmz_active %s", buffer);
                                restart++;
                        }
                }

                if(restart > 0)
                {
                        nbu_log_debug("commiting...");
			nbd_nat_dmz_setup();
                        nbd_user_config_commit();
                }
        }
#endif

        return;
}
