#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbd.h"

#include <ezxml.h>

PAGE_DF("/network/accesscontrol", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        ezxml_t xmln_root, xmln_host;
        char *buf_xml = NULL;
	size_t buf_xml_size;
        int n;


        /* Access Control Management active */
        nv_to_hdf("acm_active","acm_active");

        /* Access Control Management */
        if( nbd_user_config_xml("acm_host_list", &buf_xml, &buf_xml_size) == 0 ) {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
		{
                        n = 0;
			for(xmln_host = ezxml_child(xmln_root, "n1"); xmln_host; xmln_host = xmln_host->sibling)
			{
                                ++n;

                                nbu_string_printf(buffer, sizeof buffer, "acm_host.%d.macaddress", n);
                                ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "macaddress"));

                                nbu_string_printf(buffer, sizeof buffer, "acm_host.%d.timestart", n);
                                ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "timestart"));

                                nbu_string_printf(buffer, sizeof buffer, "acm_host.%d.timestop", n);
                                ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "timestop"));

                                nbu_string_printf(buffer, sizeof buffer, "acm_host.%d.active", n);
                                ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "active"));

                                nbu_string_printf(buffer, sizeof buffer, "acm_host.%d.behavior", n);
                                ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "behavior"));
			}

			ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }
                free(buf_xml);
        }

        /* Access Control Management Except list */
        if( nbd_user_config_xml("acm_except_list", &buf_xml, &buf_xml_size) == 0 )
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
		{
                        n = 0;
			for(xmln_host = ezxml_child(xmln_root, "n1"); xmln_host; xmln_host = xmln_host->sibling)
			{
                                	++n;
					nbu_string_printf(buffer, sizeof buffer, "acm_except.%d.url", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host,"url"));

					nbu_string_printf(buffer, sizeof buffer, "acm_except.%d.active", n);
					ewf_hdf_set(buffer, ezxml_child_txt(xmln_host,"active"));
			}

			ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }
                free(buf_xml);
        }
}

FORM_HANDLER
{
        const char * actionBuffer = NULL;
        int restart=0,n=0;

        if(ewf_hdf_sget("acm_active_submit") != NULL)
        {
                hdf_to_nv("acm_active","acm_active");
                nbd_user_config_commit();
        }
#if 0
        else if( ewf_hdf_sget("action_acm_add") != NULL )
        {
                if( (n = nbd_user_config_list_count("acm_host_list")) == -1)
                {
                        //erreur
                }
                n++;
                nbd_user_config_list_add("acm_host_list", NULL);

                nbu_string_printf(buffer, sizeof buffer, "acm_host_list_n%d_macaddress", n);
                nbd_user_config_add(buffer, ewf_hdf_sget("macaddress"));

                nbu_string_printf(buffer, sizeof buffer, "acm_host_list_n%d_timestart", n);
                nbd_user_config_add(buffer, ewf_hdf_sget("timestart"));

                nbu_string_printf(buffer, sizeof buffer, "acm_host_list_n%d_timestop", n);
                nbd_user_config_add(buffer, ewf_hdf_sget("timestop"));

                nbu_string_printf(buffer, sizeof buffer, "acm_host_list_n%d_active", n);
                if(nbu_string_compare(ewf_hdf_sget("active"),"on")  == NBU_STRING_EQUAL) {
                        nbd_user_config_add(buffer, "on");
                } else {
                        nbd_user_config_add(buffer, "off");
                }

                nbu_string_printf(buffer, sizeof buffer, "acm_host_list_n%d_behavior", n);
                nbd_user_config_add(buffer, ewf_hdf_sget("behavior"));

                restart++;
                nbd_user_config_commit();

        }
        else if( (actionBuffer = ewf_hdf_sget("action_acm_rmv")) != NULL )
        {
                nbd_user_config_list_del("acm_host_list", actionBuffer);
                nbd_user_config_commit();
        }
#endif
        else if( (actionBuffer = ewf_hdf_sget("action_disabled")) != NULL )
        {
                nbu_string_printf(buffer, sizeof buffer, "acm_host_list_n%s_active", actionBuffer);
                nbd_user_config_set(buffer,"off");
                nbd_user_config_commit();
        }
        else if( (actionBuffer = ewf_hdf_sget("action_enabled")) != NULL )
        {
                nbu_string_printf(buffer, sizeof buffer, "acm_host_list_n%s_active", actionBuffer);
                nbd_user_config_set(buffer,"on");
                nbd_user_config_commit();
        }
        else if( ewf_hdf_sget("action_except_submit") != NULL )
        {

                hdf_to_nv("url_1","acm_except_list_n1_url");
                hdf_to_nv("url_2","acm_except_list_n2_url");
                hdf_to_nv("url_3","acm_except_list_n3_url");
                hdf_to_nv("url_4","acm_except_list_n4_url");
                hdf_to_nv("url_5","acm_except_list_n5_url");

                if(nbu_string_compare(ewf_hdf_sget("active_1"), "on") == NBU_STRING_EQUAL)
                        nbd_user_config_set("acm_except_list_n1_active","on");
                else nbd_user_config_set("acm_except_list_n1_active","off");

                if(nbu_string_compare(ewf_hdf_sget("active_2"), "on") == NBU_STRING_EQUAL)
                        nbd_user_config_set("acm_except_list_n2_active","on");
                else nbd_user_config_set("acm_except_list_n2_active","off");

                if(nbu_string_compare(ewf_hdf_sget("active_3"), "on") == NBU_STRING_EQUAL)
                        nbd_user_config_set("acm_except_list_n3_active","on");
                else nbd_user_config_set("acm_except_list_n3_active","off");

                if(nbu_string_compare(ewf_hdf_sget("active_4"), "on") == NBU_STRING_EQUAL)
                        nbd_user_config_set("acm_except_list_n4_active","on");
                else nbd_user_config_set("acm_except_list_n4_active","off");

                if(nbu_string_compare(ewf_hdf_sget("active_5"), "on") == NBU_STRING_EQUAL)
                        nbd_user_config_set("acm_except_list_n5_active","on");
                else nbd_user_config_set("acm_except_list_n5_active","off");

                nbd_user_config_commit();
        }

        nbd_sys_async_run("/etc/init.d/httpfilter.sh", "restart");

        return;
}
