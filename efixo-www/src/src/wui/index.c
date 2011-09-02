#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbl_voip.h"
#include "nbl_tv.h"
#include "nbl_net.h"
#include "nbd.h"

#include <ezxml.h>

PAGE_DA_u("/index", EWF_ACCESS_PUBLIC);

DISPLAY_AJAX_HANDLER
{
        char *buf_xml = NULL;
        size_t buf_xml_size;
        ezxml_t xmln_root, xmln_gruik;

        int cpt = 0;

        /* data */
        if(nbd_status_get("net_data_access", buffer, sizeof(buffer)) == NBU_SUCCESS)
        {
                ewf_hdf_set("internet_access", buffer);
        }

	if(nbd_status_get("net_data_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("internet_status", buffer);
	}

        /* voip */
        if(nbd_status_get("net_voip_access", buffer, sizeof(buffer)) == NBU_SUCCESS)
        {
                ewf_hdf_set("voip_access", buffer);
        }

        /* net_voip_status isn't enough */
        if(nbd_status_get("voip_up", buffer, sizeof(buffer)) == NBU_SUCCESS)
	{
		ewf_hdf_set("voip_status", buffer);
	}

        /* tv */
        /* net_tv_status isn't enough */
	if(nbl_tv_get_status(buffer, sizeof(buffer)) == NBD_SUCCESS)
	{
		ewf_hdf_set("tv_status", buffer);
	}

        if(nbd_gruiks(&buf_xml, &buf_xml_size) == NBD_SUCCESS )
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        for(xmln_gruik = ezxml_child(xmln_root, "gruik");
                            xmln_gruik;
                            xmln_gruik = xmln_gruik->next)
			{
                                nbu_string_printf(buffer, sizeof buffer, "gruiks.%d", cpt);
                                ewf_hdf_set(buffer, ezxml_txt(xmln_gruik));
                                cpt++;
                        }
                        ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }

                free(buf_xml);
        }

        //IPv6
        STATUS_TO_HDF("ipv6_status");
        STATUS_TO_HDF("ipv6_lock");
        NV_TO_HDF("ipv6_enable");

	return;
}
