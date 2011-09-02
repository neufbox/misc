/*
 * mips-linux-gcc -I/home/rhk/workspace/trunk/staging/include/ -o sysfs_bus_usb sysfs_bus_usb.c /home/rhk/workspace/trunk/staging/lib/libsysfs.a
 *
 */

#include <stdio.h>
#include "libsysfs.h"

int main(int argc, char *argv[])
{
	struct sysfs_bus *sf_bus;
	struct sysfs_device *sf_dev;
	struct dlist *sf_dev_list = NULL;

	struct sysfs_attribute *sf_attr;
	struct dlist *sf_attr_list = NULL;

	sf_bus = sysfs_open_bus("usb");

	if (sf_bus != NULL)
	{
		sf_dev_list = sysfs_get_bus_devices(sf_bus);
		if (sf_dev_list != NULL)
		{
			dlist_for_each_data(sf_dev_list, sf_dev, struct sysfs_device)
			{
				printf("%s %s\n", sf_dev->name, sf_dev->path);

				//sf_attr_list = sysfs_get_device_attributes(sf_dev);
				//if (sf_attr_list != NULL)
				//{
					//dlist_for_each_data(sf_attr_list, sf_attr, struct sysfs_attribute)
					//{
						//printf("%s\n", sf_attr);
					//}
				//}

				//printf("\n");
			}
		}

		sysfs_close_bus(sf_bus);
	}
	else
	{
		printf("usb bus not found\n");
	}

	return 0;
}
