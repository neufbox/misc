/*
 * mips-linux-gcc -I/home/rhk/workspace/trunk/staging/include/ -o sysfs_driver sysfs_driver.c /home/rhk/workspace/trunk/staging/lib/libsysfs.a
 *
 */

#include <stdio.h>
#include "libsysfs.h"

int main(int argc, char *argv[])
{
	struct sysfs_driver *sf_driver;
	struct sysfs_device *sf_dev;
	struct dlist *sf_dev_list = NULL;

	sf_driver = sysfs_open_driver("usb", "usb-storage");

	if (sf_driver != NULL)
	{
		sf_dev_list = sysfs_get_driver_devices(sf_driver);
		if (sf_dev_list != NULL)
		{
			dlist_for_each_data(sf_dev_list, sf_dev, struct sysfs_device)
			{
				printf("%s %s\n", sf_dev->name, sf_dev->path);
			}
		}

		sysfs_close_driver(sf_driver);
	}
	else
	{
		printf("usb-storage driver not found\n");
	}

	return 0;
}
