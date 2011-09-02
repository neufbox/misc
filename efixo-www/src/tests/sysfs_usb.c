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

	return 0;
}

