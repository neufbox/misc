#include <ewf/ewf.h>
#include <ewf/ewf_page.h>
#include <ewf/ewf_hdf.h>

#include "nbd.h"
#include <nbd/printer.h>
#include <nbd/usharectl.h>
#include <nbd/sambactl.h>
#include <nbd/backup3g.h>

#include "nbu/nbu.h"
#include "nbu/list.h"

#include "nbl.h"

#include "dbl.h"

#include <signal.h>

#include <ezxml.h>

PAGE_DF("/state/device", EWF_ACCESS_PRIVATE);

DISPLAY_HANDLER
{
	float top34, height34, pcrt34;
	long long int capacity, capacity_used;
        const char *txt = NULL;

	int n=0, m=0, mounted = 0, flag_mounted = 0;

	char *buf_xml = NULL;
	size_t buf_xml_size;

        ezxml_t xmln_root;
        ezxml_t xmln_disk, xmln_partition;
        ezxml_t xmln_printer, xmln_ink;
        ezxml_t xmln_node;

        const char *level = NULL;
        int ilevel = 0;

	if(nbd_disk_get_list(&buf_xml, &buf_xml_size) == NBD_SUCCESS)
	{
		xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
		if(xmln_root != NULL)
		{
			n = 0;
			xmln_disk = ezxml_child(xmln_root, "disk");
			while(xmln_disk != NULL)
			{
				nbu_string_printf(buffer, sizeof buffer, "Disks.%d.usb_port", n);
				ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_disk, "usbport")));

				nbu_string_printf(buffer, sizeof buffer, "Disks.%d.devpath", n);
				ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_disk, "devpath")));

				nbu_string_printf(buffer, sizeof buffer, "Disks.%d.manufacturer", n);
				ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_disk, "manufacturer")));

				nbu_string_printf(buffer, sizeof buffer, "Disks.%d.model", n);
				ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_disk, "model")));

                                nbu_string_printf(buffer, sizeof buffer, "Disks.%d.capacity", n);
                                txt = ezxml_child_txt(xmln_disk, "capacity");
                                errno=0;
                                capacity = strtoll(txt, NULL, 10);
                                if(errno != 0)
                                {
                                        nbu_log_error("strtol(%s) failed ! %m", txt);
                                }

                                if(IS_UPPER_GB(capacity))
                                {
                                        ewf_hdf_setf(buffer, "%.1f Go", B_TO_GB(capacity));
                                }
                                else
                                {
                                        ewf_hdf_setf(buffer, "%.1f Mo", B_TO_MB(capacity));
                                }

                                flag_mounted = 0;
				m = 0;
				xmln_partition = ezxml_child(ezxml_child(xmln_disk, "partitions"), "partition");
				while(xmln_partition != NULL)
				{
					/* fs type */
					nbu_string_printf(buffer, sizeof buffer, "Disks.%d.partitions.%d.fstype", n, m);
					ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_partition, "fstype")));

					/* capacity */
                                        nbu_string_printf(buffer, sizeof buffer, "Disks.%d.partitions.%d.capacity", n, m);
                                        txt = ezxml_child_txt(xmln_partition, "capacity");
                                        errno=0;
                                        capacity = strtoll(txt, NULL, 10);
                                        if(errno != 0)
                                        {
                                                nbu_log_error("strtol(%s) failed ! %m", txt);
                                        }

                                        if(IS_UPPER_GB(capacity))
                                        {
                                                ewf_hdf_setf(buffer, "%.1f Go", B_TO_GB(capacity));
                                        }
                                        else
                                        {
                                                ewf_hdf_setf(buffer, "%.1f Mo", B_TO_MB(capacity));
                                        }

                                        /* mounted ? */
					mounted = ezxml_int(ezxml_child(xmln_partition, "mounted"), 0);
                                        nbu_string_printf(buffer, sizeof buffer, "Disks.%d.partitions.%d.mounted", n, m, mounted);
                                        ewf_hdf_set_int(buffer, mounted);

                                        if(mounted)
                                        {
                                                flag_mounted = 1;

                                                /* capacity used */
                                                nbu_string_printf(buffer, sizeof buffer, "Disks.%d.partitions.%d.capacity_used", n, m);
                                                txt = ezxml_child_txt(xmln_partition, "capacity_used");

                                                errno=0;
                                                capacity_used = strtoll(txt, NULL, 10);
                                                if(errno != 0)
                                                {
                                                        nbu_log_error("strtol(%s) failed ! %m", txt);
                                                }
                                                if(IS_UPPER_GB(capacity_used))
                                                {
                                                        ewf_hdf_setf(buffer, "%.1f Go", B_TO_GB(capacity_used));
                                                }
                                                else
                                                {
                                                        ewf_hdf_setf(buffer, "%.1f Mo", B_TO_MB(capacity_used));
                                                }

                                                if(capacity > 0)
                                                {
                                                        /* pcrt used */
                                                        nbu_string_printf(buffer, sizeof buffer, "Disks.%d.partitions.%d.pcrt_used", n, m);
                                                        ewf_hdf_setf(buffer, "%.1f", ((float)capacity_used / (float)capacity) * 100);

                                                        /* xbar. In futur, when clearsilver will do float division, move to template ... */
                                                        nbu_string_printf(buffer, sizeof buffer, "Disks.%d.partitions.%d.x_bar", n, m);
                                                        ewf_hdf_setf(buffer, "%.1f", (1-((float)capacity_used / (float)capacity)) * 350);
                                                }
                                        }

					m++;
					xmln_partition = ezxml_next(xmln_partition);
				}

				nbu_string_printf(buffer, sizeof buffer, "Disks.%d.ejected", n);
				ewf_hdf_set_int(buffer, !flag_mounted ? 1 : 0);

				n++;
				xmln_disk = ezxml_next(xmln_disk);
			}

			ezxml_free(xmln_root);
		}
		free(buf_xml);
                buf_xml = NULL;
	}

	if(nbd_printer_get_list(&buf_xml, &buf_xml_size) == NBD_SUCCESS)
	{
		xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
		if(xmln_root != NULL)
		{
			n = 0;

                        xmln_printer = ezxml_child(xmln_root, "printer");
			while(xmln_printer != NULL)
			{
                                nbu_string_printf(buffer, sizeof buffer, "Printers.%d.usb_port", n);
				ewf_hdf_set(buffer, ezxml_txt(ezxml_child(xmln_printer, "port")));

				nbu_string_printf(buffer, sizeof buffer, "Printers.%d.manufacturer", n);
				ewf_hdf_setf(buffer, ezxml_txt(ezxml_child(xmln_printer, "manufacturer")));

				nbu_string_printf(buffer, sizeof buffer, "Printers.%d.product", n);
				ewf_hdf_setf(buffer, ezxml_txt(ezxml_child(xmln_printer, "product")));

                                m = 0;
				xmln_ink = ezxml_child(ezxml_child(xmln_printer, "cartridges"), "ink");
				while(xmln_ink != NULL)
				{
                                        nbu_string_printf(buffer, sizeof buffer, "Printers.%d.cartridges.%d.type", n, m);
                                        ewf_hdf_set(buffer, ezxml_attr(xmln_ink, "name"));

                                        level = ezxml_attr(xmln_ink, "level");

                                        nbu_string_printf(buffer, sizeof buffer, "Printers.%d.cartridges.%d.level", n, m);
                                        ewf_hdf_set(buffer, level);

                                        /* compute alignment and height here
                                         * because in template, doesn't work
                                         * very well */
                                        ilevel = nbu_string_to_int(level, 0);
                                        if(ilevel < 0 || ilevel > 100)
                                        {
                                                nbu_log_error("Invalid cartridges ink level (=%d), set to 0.", ilevel);
                                                ilevel = 0;
                                        }

                                        height34 = 34 * (ilevel / 100.0);
                                        top34 = 34 - height34;
                                        pcrt34 = 17 - top34; /* 17 = 34/2 */

                                        nbu_string_printf(buffer, sizeof buffer, "Printers.%d.cartridges.%d.34height", n, m);
                                        ewf_hdf_setf(buffer, "%.1f", height34);

                                        nbu_string_printf(buffer, sizeof buffer, "Printers.%d.cartridges.%d.34top", n, m);
                                        ewf_hdf_setf(buffer, "%.1f", top34);

                                        nbu_string_printf(buffer, sizeof buffer, "Printers.%d.cartridges.%d.34pcrt", n, m);
                                        ewf_hdf_setf(buffer, "%.1f", pcrt34);

					m++;
					xmln_ink = ezxml_next(xmln_ink);
                                }

                                n++;
				xmln_printer = ezxml_next(xmln_printer);
                        }

			ezxml_free(xmln_root);
		}
		free(buf_xml);
                buf_xml = NULL;
        }

        /* gprs device */
        status_to_hdf("gprs_device_state", "gprs_device_state");
        status_to_hdf("gprs_device_fw", "gprs_device_fw");
        status_to_hdf("gprs_device_pcb", "gprs_device_pcb");
        status_to_hdf("gprs_device_imei", "gprs_device_imei");

        if(nbd_backup3g_getInfo(&buf_xml, &buf_xml_size) == NBD_SUCCESS)
        {
                xmln_root = ezxml_parse_str(buf_xml, buf_xml_size);
                if(xmln_root != NULL)
                {
                        xmln_node = ezxml_get(xmln_root, "rssi", -1);
                        ewf_hdf_set("gprs_signal_rssi", ezxml_txt(xmln_node));

                        xmln_node = ezxml_get(xmln_root, "dbm", -1);
                        ewf_hdf_set("gprs_signal_dbm", ezxml_txt(xmln_node));

                        xmln_node = ezxml_get(xmln_root, "ber", -1);
                        ewf_hdf_set("gprs_signal_ber", ezxml_txt(xmln_node));

                        ezxml_free(xmln_root);
                }
                else
                {
                        nbu_log_error("Unable to parse xml string");
                }

                free(buf_xml);
                buf_xml = NULL;
        }

	return;
}

FORM_HANDLER
{
        const char *action = NULL;
        int ret;

        EWF_HDF_GET(action);

	if(nbu_string_matches(action, "umount") == NBU_STRING_EQUAL)
	{
		if(ewf_hdf_sget_copy("devpath",
                                     buffer,
                                     sizeof  buffer) == EWF_SUCCESS)
		{
                        ret = nbd_disk_umount(buffer);
                        if(ret == disk_error_umount_failed_busy)
                        {
                                ewf_hdf_set("devpathbusy", buffer);
                                ewf_hdf_form_set_error("disk_error_umount_failed_busy");
                        }
                        else if(ret != 0)
                        {
                                ewf_hdf_form_set_error("disk_error_umount_failed");
                        }
		}
	}
        else if(nbu_string_matches(action, "umountforce") == NBU_STRING_EQUAL)
	{

                if(ewf_hdf_sget_copy("devpath",
                                     buffer,
                                     sizeof  buffer) == EWF_SUCCESS)
		{
                        ret = nbd_disk_umount_force(buffer);
                        if(ret == disk_error_umount_failed_busy)
                        {
                                ewf_hdf_set("devpathbusy", buffer);
                                ewf_hdf_form_set_error("disk_error_umount_failed_busy");
                        }
                        else if(ret != 0)
                        {
                                ewf_hdf_form_set_error("disk_error_umount_failed");
                        }
		}
        }

	return;
}
