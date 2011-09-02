#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nbu/nbu.h"
#include "nbu/string.h"
#include "nbu/list.h"
#define FINI_DMALLOC 0
#include "dmalloc.h"

typedef struct entry_test_t {
	char *value_1;
	int needed_result;
} two_entry_test_t;

#define TEST_MATCHES_S1 "hello world !"

const two_entry_test_t test_matches[] = {
	{"hello world !", NBU_STRING_EQUAL},
	{"world !", NBU_STRING_DIFFERENT},
	{"goodbye world !", NBU_STRING_DIFFERENT},
	{"he", NBU_STRING_DIFFERENT},
	{"hello", NBU_STRING_DIFFERENT},
	{NULL, 0},
};

const char *test_trim[] = {
	"",
	" \r\n \t ",
	" simple",
	" simple2 ",
	"simple3 ",
	"   simple4 ",
	" \r \n \tsimple5 ",
	" \r \n \tsi mp  l e6  \r \n \t  ",
	NULL,
};

const char *test_split[] = {
	"modules:0\n\
lan:0\n\
ifwan:0\n\
dhcpd:0\n\
www:0\n\
wlan:0\n\
dhcpc:0\n\
upnp:0\n\
",
	"10.25.15.26 15.256.25.15",
	NULL,
};

int main(void)
{
	int i = 0;
	char buf[256];
	nbu_list_t *list = NULL;
	char *p_str = NULL;
	int res;
	
	printf(" * test trim\n\n");
	
	i = 0;
	while(test_trim[i])
	{
		printf(" %d) '%s' =>", i, test_trim[i]);
		if(nbu_string_trim(test_trim[i], buf, sizeof(buf)) == NBU_SUCCESS)
		{
			printf(" '%s'\n", buf);
		}
		else
		{
			printf(" !!! FAILED !!!\n");
		}

		i++;
	}

	printf("***************************************\n");
	printf(" * test split\n\n");
	
	i = 0;
	while(test_split[i])
	{
		printf(" %d) '%s' =>", i, test_split[i]);
		if(nbu_string_split(test_split[i], " ", &list) == NBU_SUCCESS)
		{
			nbu_list_for_each_data(list, p_str, char)
			{
				printf(" '%s'", p_str);
			}
			printf("\n");
			
			nbu_list_destroy(list);
		}
		else
		{
			printf(" !!! FAILED !!!\n");
		}

		i++;
	}

	printf("***************************************\n");
	printf(" * test nbu_string_matches\n\n");
	
	i = 0;
	while(test_matches[i].value_1 != NULL)
	{
		res = nbu_string_matches(test_matches[i].value_1, TEST_MATCHES_S1);
		
		printf("%s == %s ? %d   ==> %s\n", 
		       test_matches[i].value_1,
		       TEST_MATCHES_S1,
		       res,
		       (res == test_matches[i].needed_result ? "SUCCESS" : "FAIL !!!!!!!!!!!!!!"));
		
		i++;
	}
	
	return 0;
}

