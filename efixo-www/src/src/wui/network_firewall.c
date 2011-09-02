#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

#include <ezxml.h>

#include "nbd/firewall.h"

PAGE_DF("/network/firewall", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{

        int n=0;
        ezxml_t xmln_root, xmln_host;
        char *buf_xml = NULL;
	size_t buf_xml_size;



        if (nbd_user_config_get("filter_mode", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("filter_mode", buffer);
        }

        if (nbd_user_config_get("filter_simple_winsharedrop", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("filter_simple_winsharedrop", buffer);
        }

        if (nbd_user_config_get("filter_simple_smtpdrop", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("filter_simple_smtpdrop", buffer);
        }

        if (nbd_user_config_get("filter_simple_icmpdrop", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("filter_simple_icmpdrop", buffer);
        }

        if (nbd_user_config_get("filter_simple_ipv6drop", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("filter_simple_ipv6drop", buffer);
        }


        if( nbd_user_config_xml("filter", &buf_xml, &buf_xml_size) == 0 )
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
		{
                        n = 0;
			for(xmln_host = ezxml_child(xmln_root, "rule"); xmln_host; xmln_host = xmln_host->next)
			{
                                ++n;

                                nbu_string_printf(buffer, sizeof buffer, "FilterList.%d.RuleName", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_host, "rulename")));

                                nbu_string_printf(buffer, sizeof buffer, "FilterList.%d.Proto", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_host, "proto")));

                                nbu_string_printf(buffer, sizeof buffer, "FilterList.%d.Direction", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_host, "direction")));

                                nbu_string_printf(buffer, sizeof buffer, "FilterList.%d.DstIP", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_host, "dstip")));

                                nbu_string_printf(buffer, sizeof buffer, "FilterList.%d.DstPort", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_host, "dstport")));

                                nbu_string_printf(buffer, sizeof buffer, "FilterList.%d.SrcIP", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_host, "srcip")));

                                nbu_string_printf(buffer, sizeof buffer, "FilterList.%d.SrcPort", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_host, "srcport")));

                                nbu_string_printf(buffer, sizeof buffer, "FilterList.%d.Policy", n);
                                ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_host, "policy")));
                        }
                        ezxml_free(xmln_root);
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
        const char *filter_mode = NULL,
                *filter_simple_winsharedrop = NULL,
                *filter_simple_smtpdrop = NULL,
                *filter_simple_icmpdrop = NULL,
                *filter_simple_ipv6drop = NULL;
        struct trx trx;
        int ret=0, remove_filter_id=0;
        const char *rulename,
                *proto,
                *direction,
                *dstip,
                *dstport,
                *srcip,
                *srcport,
                *policy;
        int restart=0;

        /*General Setup Filter*/
        if(ewf_hdf_sget("filter_mode") != NULL)
        {
                EWF_HDF_GET(filter_mode);
                EWF_HDF_GET_DEFAULT(filter_simple_winsharedrop, "off");
                EWF_HDF_GET_DEFAULT(filter_simple_smtpdrop, "off");
                EWF_HDF_GET_DEFAULT(filter_simple_icmpdrop, "off");
                EWF_HDF_GET_DEFAULT(filter_simple_ipv6drop, "off");

                /* trx */
                NV_TRX_INIT(trx);

                NV_SET_WTRX(trx,
                            "filter_mode", filter_mode);
                NV_SET_WTRX(trx,
                            "filter_simple_winsharedrop", filter_simple_winsharedrop);
                NV_SET_WTRX(trx,
                            "filter_simple_smtpdrop", filter_simple_smtpdrop);
                NV_SET_WTRX(trx,
                            "filter_simple_icmpdrop", filter_simple_icmpdrop);
                NV_SET_WTRX(trx,
                            "filter_simple_ipv6drop", filter_simple_ipv6drop);

                if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
                {
                        nbd_sys_sync_run(IPV6_START_STOP,
                                         "lanfilter_restart");
                        nbd_sys_sync_run(FIREWALL_START_STOP,
                                         "icmp");
                        nbd_sys_sync_run(FIREWALL_START_STOP,
                                         "smtp");
                        nbd_sys_sync_run(FIREWALL_START_STOP,
                                         "win");
                }
        }


        /*Filter rules*/
        if( (remove_filter_id = ewf_hdf_sget_int("remove_filter")) > 0 )
        {
                if(nbd_firewall_del_by_index(remove_filter_id) == NBD_SUCCESS)
                {
                        nbd_user_config_commit();
                }
        }
        if( ewf_hdf_sget("new.add") != NULL )
        {
                restart = 0;

                /* rulename */
                if((rulename = ewf_hdf_sget("new.rulename")) != NULL)
                {
                        restart++;
                }

                /* proto */
                if((proto = ewf_hdf_sget("new.proto")) != NULL)
                {
                        restart++;
                }

                /* direction */
                if((direction = ewf_hdf_sget("new.direction")) != NULL)
                {
                        restart++;
                }

                /* dstip */
                if((dstip = ewf_hdf_sget("new.dstip")) != NULL)
                {
                        restart++;
                }

                /* dstport  */
                if((dstport = ewf_hdf_sget("new.dstport")) != NULL)
                {
                        restart++;
                }

                /* srcip  */
                if((srcip = ewf_hdf_sget("new.srcip")) != NULL)
                {
                        restart++;
                }

                /* srcport  */
                if((srcport = ewf_hdf_sget("new.srcport")) != NULL)
                {
                        restart++;
                }

                /* policy  */
                if((policy = ewf_hdf_sget("new.policy")) != NULL)
                {
                        restart++;
                }

               if(restart != 0)
                {
                        if((ret = nbd_firewall_add(rulename, proto, direction, dstip, dstport, srcip, srcport, policy)) == NBD_SUCCESS)
                        {
                                nbd_user_config_commit();
                        }
                        /*else
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
                                else
                                {
                                        ewf_hdf_form_set_error("unknown");
                                }
                        }*/
                }
                else
                {
                        nbu_log_debug("nat rule insertion failed ! restart value = %i", restart);
                        ewf_hdf_form_set_error("unknown");
                }
        }




        return;
}
