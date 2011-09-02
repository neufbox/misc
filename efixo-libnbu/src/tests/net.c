#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nbu/nbu.h"
#include "nbu/net.h"
#define FINI_DMALLOC 0
#include "dmalloc.h"

const char *test_netmask[] = {
	"0.0.0.0",
	"255.255.255.256", 
	"255.255..255", 
	"2d1.255.255.255", 
	"255.255.0.255", 
	"192.0.0.0", 
	"255.252.0.0", 
	"255.255.255.224", 
	"",
	"255.255.255.255",
	"255.255.255.0",
	"255.255.0.0",
	"255.0.0.0",
	NULL,
};

const char *test_mac[] = {
	"00:0F:CB:B2:48:C9",
	"00:0F:CB:OO:48:C9",
	NULL,
};

typedef struct full_ip_t {
	char *ip;
	char *mask;
} full_ip_t;

const full_ip_t full_ip_list[] = {
	{ "192.168.0.1", "255.255.255.0", },
	{ "192.168.0.0", "255.255.255.0", },
	{ "192.168.0.255", "255.255.255.0", },
	{ "192.168.0.254", "255.255.255.0", },
	{ "192.168.0.254", "255.0.255.0", },
	{ "192.168.0.1", "255.255.240.0", },
	{ "192.168.0.0", "255.255.240.0", },
	{ "192.168.15.255", "255.255.240.0", },
	{ "192.168.16.255", "255.255.240.0", },
	{ "192.168.16.255", "255.15.0.0", },
	{ NULL, NULL, },
};

int main(void)
{
	int i = 0;
	
	printf(" * test validate netmask\n\n");
	
	i = 0;
	while(test_netmask[i])
	{
		printf(" %d) '%s' =>", i, test_netmask[i]);
		if(nbu_net_validate_netmask(test_netmask[i]) == NBU_SUCCESS)
		{
			printf(" !!! WORK !!!\n");
		}
		else
		{
			printf(" !!! FAILED !!!\n");
		}

		i++;
	}

	printf(" * test validate ip\n\n");
	
	i = 0;
	while(full_ip_list[i].ip != NULL)
	{
		printf("------------------------------------------\n");
		printf("** %s - %s => %d\n",
		       full_ip_list[i].ip,
		       full_ip_list[i].mask,
		       nbu_net_validate_full_ip(full_ip_list[i].ip, full_ip_list[i].mask));
		
		i++;
	}
	
	printf(" * test validate mac\n\n");
	
	i = 0;
	while(test_mac[i] != NULL)
	{
		printf("------------------------------------------\n");
		printf("** %s => %d\n",
		       test_mac[i],
		       nbu_net_validate_mac(test_mac[i]));
		
		i++;
	}
	
	return 0;
}
