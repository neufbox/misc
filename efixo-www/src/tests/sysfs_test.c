/*
 * mips-linux-gcc -I/home/rhk/workspace/trunk/staging/include/ -o sysfs_test sysfs_test.c /home/rhk/workspace/trunk/staging/lib/libsysfs.a
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "libsysfs.h"

int main(int argc, char *argv[])
{
	struct sysfs_bus *bus;
	struct sysfs_device *device;
	struct sysfs_class *sf_class;
	struct sysfs_class_device *sf_cdev;
	struct dlist *sf_cdev_list = NULL;
	struct dlist *list = NULL;

	bus = sysfs_open_bus("usb");
	if (bus != NULL)
	{
		list = sysfs_get_bus_devices(bus);
		if (list != NULL)
		{
			dlist_for_each_data(list, device, struct sysfs_device)
			{
				printf("%s\n", device->name);
			}
		}


		sysfs_close_bus(bus);
	}
	else
	{
		printf("bus not found\n");
	}

	sf_class = sysfs_open_class("block");

	if (sf_class != NULL)
	{
		sf_cdev_list = sysfs_get_class_devices(sf_class);
		if (sf_cdev_list != NULL)
		{
			dlist_for_each_data(sf_cdev_list, sf_cdev, struct sysfs_class_device)
			{
				printf("%s\n", sf_cdev->name);
			}
		}

		sysfs_close_class(sf_class);
	}
	else
	{
		printf("class not found\n");
	}


	return 0;
}

