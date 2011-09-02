#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbl.h"
#include "nbl_wlan.h"

#include "nbd.h"
#include "nbd/lan.h"

#include <ezxml.h>

PAGE_D("/state/lan", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        char value[32], ipaddr[NBU_NET_SIZEOF_IPADDR];
        u_int32_t rssi;

        int n;

        wl_host_diag_t *wl_host_diag = NULL;
        uint count, r;
        int b;

        ezxml_t xmln_root, xmln_port, xmln_stat, xmln_mac;

        char *buf_xml;
        size_t buf_xml_size;

#ifndef X86
        /* Get stats for the lan ports */
        if(nbd_lan_stats(&buf_xml, &buf_xml_size, "lan") == NBD_SUCCESS)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        n = 0;
                        for(xmln_port = ezxml_child(xmln_root, "port");
                            xmln_port;
                            xmln_port = xmln_port->next)
                        {
                                ++n;

                                nbu_string_printf(buffer, sizeof buffer,
                                                  "ports.%d.name", n);
                                ewf_hdf_set(buffer, ezxml_attr(xmln_port, "name"));

                                for(xmln_stat = ezxml_child(xmln_port, "counter");
                                    xmln_stat;
                                    xmln_stat = xmln_stat->next)
                                {
                                        nbu_string_printf(buffer, sizeof buffer,
                                                          "ports.%d.counters.%s", n, ezxml_attr(xmln_stat, "name"));
                                        ewf_hdf_set(buffer, ezxml_txt(xmln_stat));
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
#endif

#ifndef X86
        /* Get stats for wlan hosts */
        if(nbd_wlan_list_assoc(NULL, &buf_xml,
                               &buf_xml_size) == NBU_SUCCESS)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        n = 0;

                        for(xmln_mac = ezxml_get(xmln_root,
                                                 "assoclist",
                                                 0, "mac", -1);
                            xmln_mac;
                            xmln_mac = xmln_mac->next)
                        {
                                if (nbd_wlan_get_diag_host(&wl_host_diag,
                                                           ezxml_txt(xmln_mac)) != NBD_SUCCESS)
                                {
                                        continue;
                                }

                                ++n;

                                nbu_string_printf(buffer,
                                                  sizeof buffer,
                                                  "wlan_host.%d.macaddr",
                                                  n);
                                ewf_hdf_set(buffer, ezxml_txt(xmln_mac));

                                if(nbd_lan_mac2ip(ezxml_txt(xmln_mac),
                                                  ipaddr,
                                                  sizeof(ipaddr)) != NBD_SUCCESS)
                                {
                                        ipaddr[0] = '\0';
                                }

                                nbu_string_printf(buffer,
                                                  sizeof buffer,
                                                  "wlan_host.%d.ipaddr",
                                                  n);
                                ewf_hdf_set(buffer, ipaddr);

                                /* rates */
                                for(count = 0; count < wl_host_diag->rate_count; count++)
                                {
                                        r = wl_host_diag->rates[count] & WL_HOST_DIAG_RATE_MASK;
                                        b = wl_host_diag->rates[count] & WL_HOST_DIAG_RATE_MAGIC;
                                        if( r == 0 )
                                                break;

                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.rates.%i", n, count);
                                        nbu_string_printf(value, sizeof value,
                                                          "%d%s%s ",
                                                          (r / 2),
                                                          (r % 2)?".5" : "",
                                                          b ?"(b)" : "");
                                        ewf_hdf_set(buffer, value);
                                }

                                /* idle */
                                nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.idle", n);
                                nbu_string_printf(value, sizeof value, "%u", wl_host_diag->idle);
                                ewf_hdf_set(buffer, value);

                                /* in */
                                nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.in", n);
                                nbu_string_printf(value, sizeof value, "%u", wl_host_diag->in);
                                ewf_hdf_set(buffer, value);

                                /* state */
                                nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.state", n);
                                nbu_string_printf(value, sizeof value,
                                                  "%s%s%s",
                                                  (wl_host_diag->flags & WL_HOST_DIAG_AUTHE) ? " AUTHENTICATED" : "",
                                                  (wl_host_diag->flags & WL_HOST_DIAG_ASSOC) ? " ASSOCIATED" : "",
                                                  (wl_host_diag->flags & WL_HOST_DIAG_AUTHO) ? " AUTHORIZED" : "");
                                ewf_hdf_set(buffer, value);

                                /* flags */
                                nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.flags", n);
                                nbu_string_printf(value, sizeof value,
                                                  "0x%x:%s%s%s%s%s%s%s%s%s",
                                                   wl_host_diag->flags,
                                                   (wl_host_diag->flags & WL_HOST_DIAG_BRCM) ? " BRCM" : "",
                                                   (wl_host_diag->flags & WL_HOST_DIAG_ABCAP) ? " ABCAP" : "",
                                                   (wl_host_diag->flags & WL_HOST_DIAG_WME) ? " WME" : "",
                                                   (wl_host_diag->flags & WL_HOST_DIAG_PS) ? " PS" : "",
                                                   (wl_host_diag->flags & WL_HOST_DIAG_APSD_BE) ? " APSD_BE" : "",
                                                   (wl_host_diag->flags & WL_HOST_DIAG_APSD_BK) ? " APSD_BK" : "",
                                                   (wl_host_diag->flags & WL_HOST_DIAG_APSD_VI) ? " APSD_VI" : "",
                                                   (wl_host_diag->flags & WL_HOST_DIAG_APSD_VO) ? " APSD_VO" : "",
                                                   (wl_host_diag->flags & WL_HOST_DIAG_N_CAP) ? " N_CAP" : "");
                                ewf_hdf_set(buffer, value);

                                /* extra info */
                                if(wl_host_diag->flags & WL_HOST_DIAG_EXTRA_INFO)
                                {
                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.extra_info", n);
                                        ewf_hdf_set(buffer, "1");

                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.tx_pkts", n);
                                        nbu_string_printf(value, sizeof value, "%u", wl_host_diag->tx_pkts);
                                        ewf_hdf_set(buffer, value);

                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.tx_failures", n);
                                        nbu_string_printf(value, sizeof value, "%u", wl_host_diag->tx_failures);
                                        ewf_hdf_set(buffer, value);

                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.rx_ucast_pkts", n);
                                        nbu_string_printf(value, sizeof value, "%u", wl_host_diag->rx_ucast_pkts);
                                        ewf_hdf_set(buffer, value);

                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.rx_mcast_pkts", n);
                                        nbu_string_printf(value, sizeof value, "%u", wl_host_diag->rx_mcast_pkts);
                                        ewf_hdf_set(buffer, value);

                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.tx_rate", n);
                                        nbu_string_printf(value, sizeof value, "%u bits/s", wl_host_diag->tx_rate);
                                        ewf_hdf_set(buffer, value);

                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.rx_rate", n);
                                        nbu_string_printf(value, sizeof value, "%u bits/s", wl_host_diag->rx_rate);
                                        ewf_hdf_set(buffer, value);
                                }

                                /* rssi */
                                if(nbd_wlan_get_rssi(&rssi, ezxml_txt(xmln_mac)) == NBD_SUCCESS)
                                {
                                        nbu_string_printf(buffer, sizeof buffer, "wlan_host.%d.rssi", n);
                                        nbu_string_printf(value, sizeof value, "%d", rssi);
                                        ewf_hdf_set(buffer, value);
                                }

                                free(wl_host_diag);
                        }


                        ezxml_free(xmln_root);
                }

                free(buf_xml);
        }
#endif

        return;
}
