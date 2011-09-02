/*
 * mips-linux-gcc -I/home/rhk/workspace/trunk/staging/include/ -o sysfs_class_block sysfs_class_block.c /home/rhk/workspace/trunk/staging/lib/libsysfs.a
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
	char buffer[64];
	int port;

	struct sysfs_attribute *sf_attr;
	struct sysfs_attribute *sf_part;
	struct dlist *sf_part_list = NULL;

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
					port = *(strstr(sf_dev->path, "/usb1/") + 8) - 48;
					if ((port != 1) && (port != 2))
					{
						port = -1;
					}

					printf("%s, USB port %d\n", sf_cdev->name, port);

					/* size */
					snprintf(buffer, 64, "%s/size", sf_cdev->path);

					sf_attr = sysfs_open_attribute(buffer);
					if (sf_attr != NULL)
					{
						if (sysfs_read_attribute(sf_attr) == 0)
						{
							printf("size: %s", sf_attr->value);
						}
						sysfs_close_attribute(sf_attr);
					}

					/* vendor */
					snprintf(buffer, 64, "%s/device/vendor", sf_cdev->path);

					sf_attr = sysfs_open_attribute(buffer);
					if (sf_attr != NULL)
					{
						if (sysfs_read_attribute(sf_attr) == 0)
						{
							printf("vendor: %s", sf_attr->value);
						}
						sysfs_close_attribute(sf_attr);
					}

					/* model */
					snprintf(buffer, 64, "%s/device/model", sf_cdev->path);

					sf_attr = sysfs_open_attribute(buffer);
					if (sf_attr != NULL)
					{
						if (sysfs_read_attribute(sf_attr) == 0)
						{
							printf("model: %s", sf_attr->value);
						}
						sysfs_close_attribute(sf_attr);
					}

					sf_part_list = sysfs_open_directory_list(sf_cdev->path);
					if (sf_part_list != NULL)
					{
						dlist_for_each_data(sf_part_list, sf_part, struct sysfs_attribute)
						{
							if (strncmp("queue", (char *) sf_part, 5) != 0)
							{
								snprintf(buffer, 64, "%s/%s/size", sf_cdev->path, sf_part);

								sf_attr = sysfs_open_attribute(buffer);
								if (sf_attr != NULL)
								{
									if (sysfs_read_attribute(sf_attr) == 0)
									{
										printf("%s - size: %s", sf_part, sf_attr->value);
									}
									sysfs_close_attribute(sf_attr);
								}
							}
						}
					}

					printf("\n");
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
