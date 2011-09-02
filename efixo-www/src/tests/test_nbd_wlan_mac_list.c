/*
 * $ mips-linux-uclibc-gcc -I../../../../staging/include test_nbd_wlan_mac_list.c -L../../../../staging/lib -Wl,-Bstatic -lsysfs -Wl,-Bdynamic -lnbd -o test_nbd_wlan_mac_list
 */

#include "nbd.h"
#include "nbu/list.h"

nbu_list_t *nbd_wlan_mac_list()
{
	nbu_list_t *list = NULL;
	char *cur;
	char *maclist;
	char *mac;

	if (nbd_wlan_listmac(&maclist) == NBD_SUCCESS)
	{
		if (maclist != NULL)
		{
			list = nbu_list_new(sizeof(char *));
			if (list != NULL)
			{
				mac = maclist;
				while (*mac)
				{
					cur = (char *) strndup(mac, 17);
					nbu_list_insert_end(list, cur);
					mac += NBD_ETHER_SIZE;
				}
			}

			free(maclist);
		}
	}

	return list;
}

int main()
{
	nbu_list_t *mac_list = NULL;
	char *mac = NULL;
	int n;

	nbd_connect();

	mac_list = nbd_wlan_mac_list();
	if (mac_list != NULL)
	{
		nbu_list_for_each_data(mac_list, mac, char)
		{
			printf("%s\n", mac);
		}

		nbu_list_terminator(mac_list);
	}

	nbd_disconnect();

	return 0;
}
