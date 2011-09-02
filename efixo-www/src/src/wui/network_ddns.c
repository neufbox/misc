#include <time.h>
#include <string.h>

#include <ezxml.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbd.h"
#include "dbl.h"

PAGE_DFA_u("/network/ddns", EWF_ACCESS_PRIVATE);

DISPLAY_AJAX_HANDLER
{
        char host[256];
        char domain[256];

        char *buf_xml = NULL;
        size_t buf_xml_size;
        ezxml_t xmln_root;

        //gprs/ppp ?
        status_to_hdf("net_data_infra", "net_data_infra");

        if (nbd_status_get("net_data_status", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                ewf_hdf_set("internet_status", buffer);
        }

        if( nbd_ddnsctl_status(&buf_xml, &buf_xml_size) == NBD_SUCCESS )
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        ewf_hdf_set("ddns_active", ezxml_txt(ezxml_child(xmln_root, "active")));
                        ewf_hdf_set("ddns_status", ezxml_txt(ezxml_child(xmln_root, "status")));
                        ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }

                free(buf_xml);
        }

        nv_to_hdf("ddns_username", "ddns_username");
        nv_to_hdf("ddns_password", "ddns_password");

        if (nbd_user_config_get("ddns_hostname", buffer, sizeof(buffer)) == NBD_SUCCESS)
        {
                if (nbu_net_split_hostname(buffer, host, sizeof(host), domain, sizeof(domain)) == 0)
                {
                        ewf_hdf_set("ddns_hostname_p0", host);
                        ewf_hdf_set("ddns_hostname_p1", domain);
                }
        }

        return;
}

FORM_HANDLER
{
        const char *ddns_active = NULL, *ddns_username = NULL,
                *ddns_password = NULL,
                *ddns_hostname_p0 = NULL, *ddns_hostname_p1 = NULL;
        char ddns_hostname[256];
        struct trx trx;

        /* get hdf query values */
        EWF_HDF_GET(ddns_active);
        EWF_HDF_GET(ddns_username);
        EWF_HDF_GET(ddns_password);
        EWF_HDF_GET(ddns_hostname_p0);
        EWF_HDF_GET(ddns_hostname_p1);

        /* compute some stuff */
        if (nbu_net_join_hostname(ddns_hostname_p0,
                                  ddns_hostname_p1,
                                  ddns_hostname,
                                  sizeof(ddns_hostname)) != NBU_SUCCESS)
        {
                nbu_log_error("join_hostname failed (%s, %s)",
                              ddns_hostname_p0, ddns_hostname_p1);
                ewf_hdf_query_var_error("ddns_hostname", "value");
                ewf_hdf_query_replay();
                return;
        }

        /* session init, set and commit/close */
        NV_TRX_INIT(trx);

        NV_SET_WTRX(trx, "ddns_active", ddns_active);
        NV_SET_WTRX(trx, "ddns_username", ddns_username);
        NV_SET_WTRX(trx, "ddns_password", ddns_password);
        NV_SET_WTRX(trx, "ddns_hostname", ddns_hostname);

        if(NV_TRX_XCOMMIT(trx) == NV_COMMIT_SUCCESS)
        {
                nbd_sys_sync_run(DDNSD_START_STOP, "restart");
        }

        return;
}
