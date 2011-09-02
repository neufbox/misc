#include <string.h>
#include <ezxml.h>

#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>
#include <ewf/core/ewf_fcgi.h>

#include <nbd/sambactl.h>

#include "nbl.h"
#include "nbd.h"
#include "dbl.h"

/* samba page */

PAGE_DFA("/service/samba", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
        char *buf_xml = NULL, *buf_xml2 = NULL;
        size_t buf_xml_size, buf_xml2_size;
        ezxml_t xmln_root, xmln, xmln_disk, xmln_partition,
                xmln_root_dir, xmln_dir;

        double capacity;
        const char *uuid = NULL;
        const char *txt = NULL;

        int n, m;

        /* get status of service */
        if( nbd_sambactl_status(&buf_xml, &buf_xml_size) == NBD_SUCCESS )
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        ewf_hdf_set("samba_active", ezxml_child_txt(xmln_root, "active"));
                        ewf_hdf_set("samba_state", ezxml_child_txt(xmln_root, "state"));
                        ewf_hdf_set("samba_codeno", ezxml_child_txt(xmln_root, "codeno"));

                        ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }

                free(buf_xml);
        }

        nv_to_hdf("samba_workgroup", "samba_workgroup");
        nv_to_hdf("samba_name", "samba_name");

        /* get list of shares */
        if (nbd_user_config_xml("samba", &buf_xml, &buf_xml_size) == 0)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                for ( n = 1, xmln = ezxml_child(xmln_root, "share");
                      xmln;
                      n++, xmln = xmln->next )
                {
                        uuid = ezxml_child_txt(xmln, "uuid");

                        /* uuid */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "samba_stockage_list.%d.uuid",
                                          n);
                        ewf_hdf_set(buffer, uuid);

                        /* name */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "samba_stockage_list.%d.name",
                                          n);
                        ewf_hdf_set(buffer,
                                    ezxml_child_txt(xmln, "name"));

                        /* dir */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "samba_stockage_list.%d.dir",
                                          n);
                        ewf_hdf_set(buffer,
                                    ezxml_child_txt(xmln, "dir"));

                        /* online ? */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "samba_stockage_list.%d.online",
                                          n);
                        ewf_hdf_set_int(buffer,
                                        nbd_disk_part_uuid_is_online(uuid));
                }

                ezxml_free(xmln_root);
                free(buf_xml);
        }

        /* get info of stockages used on system */
        if (nbd_user_config_xml("stockage", &buf_xml, &buf_xml_size) == 0)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                for ( xmln = ezxml_child(xmln_root, "device");
                      xmln;
                      xmln = xmln->next )
                {
                        uuid = ezxml_child_txt(xmln, "uuid");

                        /* uuid */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "stockage_list.%s.uuid",
                                          uuid);
                        ewf_hdf_set(buffer, uuid);

                        /* pname */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "stockage_list.%s.pname",
                                          uuid);
                        ewf_hdf_set(buffer,
                                    ezxml_child_txt(xmln, "pname"));

                        /* type */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "stockage_list.%s.type",
                                          uuid);
                        ewf_hdf_set(buffer,
                                    ezxml_child_txt(xmln, "type"));

                        /* capacity */
                        txt = ezxml_child_txt(xmln, "capacity");
                        errno=0;
                        capacity = strtoll(txt, NULL, 10);
                        if(errno != 0)
                        {
                                nbu_log_error("strtol(%s) failed ! %m", txt);
                                capacity = 0;
                        }
                        nbu_string_printf(buffer, sizeof buffer,
                                          "stockage_list.%s.capacity",
                                          uuid);
                        if(IS_UPPER_GB(capacity))
                        {
                                ewf_hdf_setf(buffer, "%.1f Go",
                                             B_TO_GB(capacity));
                        }
                        else
                        {
                                ewf_hdf_setf(buffer, "%.1f Mo",
                                             B_TO_MB(capacity));
                        }
                }

                ezxml_free(xmln_root);
                free(buf_xml);
        }

        /* get partitions online */
        if(nbd_disk_get_list(&buf_xml, &buf_xml_size) == NBD_SUCCESS)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                for(n = 1, xmln_disk = ezxml_child(xmln_root, "disk");
                    xmln_disk;
                    n++, xmln_disk = ezxml_next(xmln_disk))
                {
                        /* usb port */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "disks.%d.usb_port",
                                          n);
                        ewf_hdf_set(buffer,
                                    ezxml_child_txt(xmln_disk,
                                                    "usbport"));

                        /* vendor */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "disks.%d.manufacturer",
                                          n);
                        ewf_hdf_set(buffer,
                                    ezxml_child_txt(xmln_disk,
                                                    "manufacturer"));

                        /* model */
                        nbu_string_printf(buffer, sizeof buffer,
                                          "disks.%d.model",
                                          n);
                        ewf_hdf_set(buffer,
                                    ezxml_child_txt(xmln_disk,
                                                    "model"));

                        /* list of partitions */
                        xmln = ezxml_child(xmln_disk, "partitions");
                        for(m = 1, xmln_partition = ezxml_child(xmln, "partition");
                            xmln_partition;
                            m++, xmln_partition = ezxml_next(xmln_partition))
                        {
                                /* only get mounted partition */
                                if(ezxml_int(ezxml_child(xmln_partition, "mounted"), 0) == 1)
                                {
                                        /* fs type */
                                        nbu_string_printf(buffer, sizeof buffer,
                                                          "disks.%d.partitions.%d.fstype",
                                                          n, m);
                                        ewf_hdf_set(buffer,
                                                    ezxml_child_txt(xmln_partition, "fstype"));

                                        /* mnt point */
                                        nbu_string_printf(buffer, sizeof buffer,
                                                          "disks.%d.partitions.%d.mntpath",
                                                          n, m);
                                        ewf_hdf_set(buffer,
                                                    ezxml_child_txt(xmln_partition, "mntpath"));

                                        /* capacity */
                                        txt = ezxml_child_txt(xmln_partition, "capacity");
                                        errno=0;
                                        capacity = strtoll(txt, NULL, 10);
                                        if(errno != 0)
                                        {
                                                nbu_log_error("strtol(%s) failed ! %m", txt);
                                                capacity = 0;
                                        }
                                        nbu_string_printf(buffer, sizeof buffer,
                                                          "disks.%d.partitions.%d.capacity",
                                                          n, m);
                                        if(IS_UPPER_GB(capacity))
                                        {
                                                ewf_hdf_setf(buffer, "%.1f Go", B_TO_GB(capacity));
                                        }
                                        else
                                        {
                                                ewf_hdf_setf(buffer, "%.1f Mo", B_TO_MB(capacity));
                                        }

                                        /* uuid */
                                        uuid = ezxml_child_txt(xmln_partition, "uuid");
                                        nbu_string_printf(buffer, sizeof buffer,
                                                          "disks.%d.partitions.%d.uuid",
                                                          n, m);
                                        ewf_hdf_set(buffer, uuid);

                                        /* dir listing */
                                        if(nbd_disk_get_dirlisting(uuid,
                                                                   &buf_xml2,
                                                                   &buf_xml2_size) == 0)
                                        {
                                                xmln_root_dir = ezxml_parse_str(buf_xml2,
                                                                                buf_xml2_size);
                                                for(xmln_dir = ezxml_child(xmln_root_dir, "dir");
                                                    xmln_dir;
                                                    xmln_dir = ezxml_next(xmln_dir))
                                                {
                                                        nbu_string_printf(buffer, sizeof buffer,
                                                                          "disks.%d.partitions.%d.dirs.%p.name",
                                                                          n, m, xmln_dir);
                                                        ewf_hdf_set(buffer, ezxml_txt(xmln_dir));
                                                }

                                                ezxml_free(xmln_root_dir);
                                                free(buf_xml2);
                                        }
                                }
                        }
                }

                ezxml_free(xmln_root);
                free(buf_xml);
        }

        return;
}

FORM_HANDLER {
        const char *action = NULL,
                *stock_name = NULL,
                *stock_share = NULL,
                *stock_uuid = NULL,
                *stock_dir = NULL,
                *idx = NULL;
        nbu_list_t *list_arg = NULL;
        int i = 0;
        int restart = 0;
        int ret;

        action = ewf_hdf_get("action");
        if(action != NULL)
        {
                if(nbu_string_compare(action, "addshare") == NBU_STRING_EQUAL)
                {
                        stock_name = ewf_hdf_sget("samba_stockage_name");
                        stock_share = ewf_hdf_sget("samba_stockage_share");

                        if(stock_name != NULL && stock_share != NULL)
                        {
                                if(nbu_string_split(stock_share, ",", &list_arg) == NBU_SUCCESS
                                   && nbu_list_count(list_arg) == 2)
                                {
                                        stock_uuid = nbu_list_get_at_index(list_arg, 0);
                                        stock_dir = nbu_list_get_at_index(list_arg, 1);

                                        ret = nbd_sambactl_add_share(stock_name, stock_uuid, stock_dir);
                                        if(ret == NBD_SUCCESS)
                                        {
                                                restart ++;
                                        }
                                        else if(ret == sambactl_error_invalid_name)
                                        {
                                                ewf_hdf_set_error("samba_stockage_name", "invalid_name");
                                        }
                                        else
                                        {
                                                nbu_log_error("Error !");
                                                ewf_hdf_form_set_error("unknown");
                                        }
                                }
                                else
                                {
                                        nbu_log_error("Invalid samba_stockage_share ! it's not normal guy !");
                                        ewf_hdf_form_set_error("unknown");
                                }

                                nbu_list_destroy(list_arg);
                        }
                }
                else if(nbu_string_compare(action, "delshare") == NBU_STRING_EQUAL)
                {
                        idx = ewf_hdf_sget("samba_share_idx");

                        if(idx != NULL)
                        {
                                errno = 0;
                                i = strtol(idx, NULL, 10);

                                if(errno != 0 || nbd_sambactl_del_share(i) != NBD_SUCCESS)
                                {
                                        nbu_log_error("Error !");
                                        ewf_hdf_form_set_error("unknown");
                                }
                                else
                                {
                                        restart++;
                                }
                        }
                }
                else
                {
                        if(hdf_to_nv("samba_active", "samba_active") == NV_SET_SUCCESS)
                        {
                                restart++;
                        }

                        if(hdf_to_nv("samba_name", "samba_name") == NV_SET_SUCCESS)
                        {
                                restart++;
                        }

                        if(hdf_to_nv("samba_workgroup", "samba_workgroup") == NV_SET_SUCCESS)
                        {
                                restart++;
                        }
                }

                if(restart > 0)
                {
                        nbd_user_config_commit();
                        nbd_sambactl_restart();
                }
        }
}

AJAX_HANDLER
{
        char *buf_xml = NULL;
        size_t buf_xml_size;
        ezxml_t xmln_root;

        if( nbd_sambactl_status(&buf_xml, &buf_xml_size) == NBD_SUCCESS )
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        ewf_hdf_set("samba_active",
                                    ezxml_child_txt(xmln_root, "active"));

                        ewf_hdf_set("samba_state",
                                    ezxml_child_txt(xmln_root, "state"));

                        ewf_hdf_set("samba_codeno",
                                    ezxml_child_txt(xmln_root, "codeno"));

                        ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }

                free(buf_xml);
        }
}
