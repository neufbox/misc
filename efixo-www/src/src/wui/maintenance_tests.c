#include <string.h>
#include <ezxml.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/ewf_utils.h>
#include <ewf/core/ewf_fcgi.h>

#include "nbl.h"
#include "nbl_net.h"
#include "nbd.h"

PAGE_DA("/maintenance/tests", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	return;
}

AJAX_HANDLER
{
#ifndef X86
	nbd_ping_status_t status;
	struct tr_result traceroute_result;
	int i = 0, i_id = -1, ret = NBU_ERROR;
	const char* action = NULL, *run = NULL, *id = NULL, *dest_hostname = NULL;
	char ip_src[16];

	ewf_hdf_cgi_parse();

        action = ewf_hdf_sget("action");
        run = ewf_hdf_sget("run");

	if(action == NULL || run == NULL)
	{
                nbu_log_error("Need more arguments");
		return;
	}

        ewf_hdf_set("action", action);

	if(nbu_string_matches(action, "ping") == NBU_STRING_EQUAL)
	{
		/* Ping */
		if(nbu_string_matches(run, "start") == NBU_STRING_EQUAL)
		{
                        dest_hostname = ewf_hdf_sget("ping_dest_hostname");
                        if(dest_hostname == NULL)
                        {
                                nbu_log_error("ping_dest_hostname NULL");
                                ewf_hdf_set("ping_status", "error");
                                return;
                        }

                        ret = nbl_net_get_ipsrc_from_ipdest(dest_hostname,
                                                            ip_src,
                                                            sizeof(ip_src));

                        if(ret != NBL_NET_GETIPSRC_SUCCESS)
                        {
                                nbu_log_error("Unable to get ip src for joining %s",
                                              dest_hostname);

                                if(ret == NBL_NET_GETIPSRC_ERR_DNS_TMPFAIL)
                                {
                                        ewf_hdf_set("ping_status", "error_dns_tempfail");
                                }
                                else if(ret == NBL_NET_GETIPSRC_ERR_DNS_UNKNOWNDEST)
                                {
                                        ewf_hdf_set("ping_status", "error_dns_host");
                                }
                                else if(ret == NBL_NET_GETIPSRC_ERR_UNKNOWN)
                                {
                                        ewf_hdf_set("ping_status", "error");
                                }

                                return;
                        }

			if(nbd_ping_start(&i_id,
                                          ip_src,
                                          dest_hostname,
                                          10) != -1)
			{
				/* Start ping */
                                ewf_hdf_set("ping_status", "living");
                                ewf_hdf_set_int("ping_id", i_id);
			}
			else
			{
                                ewf_hdf_set("ping_status", "error");
			}
		}
		else if(nbu_string_matches(run, "stop") == NBU_STRING_EQUAL)
		{
			if((id = ewf_hdf_sget("id")) != NULL
			   && nbd_ping_stop(atoi(id)) != -1)
			{
                                ewf_hdf_set("ping_status", "stop");
			}
			else
			{
                                ewf_hdf_set("ping_status", "error");
			}
		}
		else
		{
			if ((id = ewf_hdf_sget("id")) != NULL
			    && nbd_ping_status(atoi(id), &status) != -1)
			{
				if(status.done)
				{
                                        ewf_hdf_set("ping_status", "finished");
				}
				else
				{
                                        ewf_hdf_set("ping_status", "living");
				}

                                ewf_hdf_set_int("ping_sent", status.sent);
                                ewf_hdf_set_int("ping_received", status.received);
                                ewf_hdf_set_int("ping_avgrtt", status.avgrtt);
			}
			else
			{
                                ewf_hdf_set("ping_status", "halt");
			}
		}

	}
	else if(nbu_string_matches(action, "traceroute") == NBU_STRING_EQUAL)
	{
		/* Traceroute */
		if(nbu_string_matches(run, "start") == NBU_STRING_EQUAL)
		{
                        dest_hostname = ewf_hdf_sget("traceroute_dest_hostname");
                        if(dest_hostname == NULL)
                        {
                                nbu_log_error("ping_dest_hostname NULL");
                                ewf_hdf_set("traceroute_status", "error");
                                return;
                        }

                        ret = nbl_net_get_ipsrc_from_ipdest(dest_hostname,
                                                            ip_src,
                                                            sizeof(ip_src));

                        if(ret != NBL_NET_GETIPSRC_SUCCESS)
                        {
                                nbu_log_error("Unable to get ip src for joining %s",
                                              dest_hostname);

                                if(ret == NBL_NET_GETIPSRC_ERR_DNS_TMPFAIL)
                                {
                                        ewf_hdf_set("traceroute_status", "error_dns_tempfail");
                                }
                                else if(ret == NBL_NET_GETIPSRC_ERR_DNS_UNKNOWNDEST)
                                {
                                        ewf_hdf_set("traceroute_status", "error_dns_host");
                                }
                                else if(ret == NBL_NET_GETIPSRC_ERR_UNKNOWN)
                                {
                                        ewf_hdf_set("traceroute_status", "error");
                                }

                                return;
                        }

			if(nbd_traceroute_start(&i_id, ip_src,
                                                dest_hostname) != -1)
			{
				/* Start traceroute */
				nbu_log_debug("Traceroute start : %s --> %s",
                                              ip_src, dest_hostname);

                                ewf_hdf_set("traceroute_status", "living");
                                ewf_hdf_set_int("traceroute_id", i_id);
			}
			else
			{
				ewf_hdf_set("traceroute_status", "error");
			}
		}
		else if(nbu_string_matches(run, "stop") == NBU_STRING_EQUAL)
		{
			/* Stop traceroute */
			if((id = ewf_hdf_sget("id")) != NULL
			   && nbd_traceroute_stop(atoi(id)) != -1)
			{
				ewf_hdf_set("traceroute_status", "stop");
			}
			else
			{
				ewf_hdf_set("traceroute_status", "error");
			}

		}
		else
		{
			/* Traceroute last value */
			if((id = ewf_hdf_sget("id")) != NULL
			   && nbd_traceroute_get(atoi(id), &traceroute_result) != -1)
			{
				if(traceroute_result.finished)
				{
                                        ewf_hdf_set("traceroute_status", "finished");
				}
				else
				{
                                        ewf_hdf_set("traceroute_status", "living");
				}

                                ewf_hdf_set("traceroute_target",
                                            traceroute_result.dst_ip != NULL ?
                                            traceroute_result.dst_ip : "Unknown");

				for(i = 0; i < traceroute_result.hop_count; i++)
				{
                                        nbu_string_printf(buffer, sizeof(buffer),
                                                          "traceroute_hops.%d.index", i);
                                        ewf_hdf_set_int(buffer, i + 1);

                                        nbu_string_printf(buffer, sizeof(buffer),
                                                          "traceroute_hops.%d.hostname", i);
                                        ewf_hdf_set(buffer,
                                                    traceroute_result.hops[i].hostname != NULL ?
                                                    traceroute_result.hops[i].hostname : traceroute_result.hops[i].ip);

                                        nbu_string_printf(buffer, sizeof(buffer),
                                                          "traceroute_hops.%d.ip", i);
                                        ewf_hdf_set(buffer, traceroute_result.hops[i].ip);

                                        nbu_string_printf(buffer, sizeof(buffer),
                                                          "traceroute_hops.%d.rtt", i);
                                        ewf_hdf_set_int(buffer, traceroute_result.hops[i].rtt[0]);
				}
			}
			else
			{
				ewf_hdf_set("traceroute_status", "halt");
			}
		}
	}
#endif

	return;
}
