/*
 * mips-linux-gcc -I/home/rhk/workspace/trunk/staging/include/ -o sysfs_list sysfs_list.c /home/rhk/workspace/trunk/staging/lib/libsysfs.a
 *
 */

#include <stdio.h>
#include "libsysfs.h"

int main(int argc, char *argv[])
{
	struct sysfs_driver *sf_driver;
	struct sysfs_bus *sf_bus;
	struct sysfs_device *sf_dev;
	struct sysfs_device *sf_bdev;
	struct sysfs_attribute *sf_attr;
	struct dlist *sf_dev_list = NULL;

	sf_driver = sysfs_open_driver("usb", "usb-storage");

	if (sf_driver != NULL)
	{
		sf_dev_list = sysfs_get_driver_devices(sf_driver);
		if (sf_dev_list != NULL)
		{
			sf_bus = sysfs_open_bus("usb");
			if (sf_bus != NULL)
			{
				dlist_for_each_data(sf_dev_list, sf_dev, struct sysfs_device)
				{
					printf("%s\n", sf_dev->name);

					sf_bdev = sysfs_get_bus_device(sf_bus, sysfs_get_device_parent(sf_dev)->bus_id);
					if (sf_bdev != NULL)
					{
						sf_attr = sysfs_get_device_attr(sf_bdev, "speed");
						printf("%s\n", sf_attr->value);
					}

					printf("\n");
				}
			}

			sysfs_close_bus(sf_bus);
		}

		sysfs_close_driver(sf_driver);
	}
	else
	{
		printf("usb-storage driver not found\n");
	}

	return 0;
}
