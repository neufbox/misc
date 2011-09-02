#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>

#include <nbd/config.h>

#include "nbl.h"
#include "nbl_plug.h"
#include "nbl_net.h"

#include "nbd.h"

#include <ezxml.h>

PAGE_DF("/state/device/plug", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{

        ezxml_t xmln_root, xmln_host;

        char *buf_xml = NULL;
        size_t buf_xml_size;

        int n;
        /* lan topology */
        if(nbd_status_xml("lan_topology", &buf_xml, &buf_xml_size) == NBD_SUCCESS)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        n = 0;
                        for(xmln_host = ezxml_child(xmln_root, "device"); xmln_host; xmln_host = xmln_host->next)
                        {
                                if(nbu_string_compare(ezxml_child_txt(xmln_host, "type"), "plc") == NBU_SUCCESS)
                                {
                                        ++n;

                                        nbu_string_printf(buffer, sizeof buffer, "hosts.%d.name", n);
                                        ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "hostname"));

                                        nbu_string_printf(buffer, sizeof buffer, "hosts.%d.ip", n);
                                        ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "ipaddr"));

                                        nbu_string_printf(buffer, sizeof buffer, "hosts.%d.mac", n);
                                        ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "macaddr"));

                                        nbu_string_printf(buffer, sizeof buffer, "hosts.%d.iface", n);
                                        ewf_hdf_set(buffer, ezxml_child_txt(xmln_host, "port"));
                                }
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
        const char *cpl_master = NULL;
        const char *cpl_slave = NULL;
        int i = 0;
        char buf[512];
        
        EWF_HDF_GET(cpl_master);
        EWF_HDF_GET(cpl_slave);
        
        ewf_hdf_set("cpl_master_selected", cpl_master);
        ewf_hdf_set("cpl_slave_selected", cpl_slave);

        while(nbl_plug_get_stats(buf, sizeof buf, cpl_master, cpl_slave) >= 0)
        {
                nbu_string_printf(buffer, sizeof buffer, "CPL_plugs.part%d", i);
                ewf_hdf_set(buffer, buf);
                ++i;
        }

        nbl_plug_get_info_speed(buf, sizeof buf, cpl_master, cpl_slave);
        ewf_hdf_set("CPL_plugs_speed", buf);
        
        nbl_plug_get_info_rate(buf, sizeof buf, cpl_master, cpl_slave);
        ewf_hdf_set("CPL_plugs_rate", buf);

        return;
}
