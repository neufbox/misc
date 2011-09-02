/*
 * mips-linux-gcc -I/home/rhk/workspace/trunk/staging/include/ -o sysfs_block sysfs_block.c /home/rhk/workspace/trunk/staging/lib/libsysfs.a
 *
 */

#include <stdio.h>
#include "libsysfs.h"

int main(int argc, char *argv[])
{
	struct sysfs_class *sf_class;
	struct sysfs_device *sf_dev;
	struct sysfs_class_device *sf_cdev;
	struct dlist *sf_cdev_list = NULL;

	sf_class = sysfs_open_class("block");

	if (sf_class != NULL)
	{
		sf_cdev_list = sysfs_get_class_devices(sf_class);
		if (sf_cdev_list != NULL)
		{
			dlist_for_each_data(sf_cdev_list, sf_cdev, struct sysfs_class_device)
			{
				sf_dev = sysfs_get_classdev_device(sf_cdev);
				if (sf_dev != NULL)
				{
					printf("%s -> %s\n", sf_cdev->name, sf_dev->name);
				}
			}
		}

		sysfs_close_class(sf_class);
	}
	else
	{
		printf("block class not found\n");
	}

	return 0;
}
