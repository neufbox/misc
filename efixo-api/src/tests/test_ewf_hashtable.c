#include "ewf.h"

/* gcc -I../src ../src/ewf_hashtable.c ../src/ewf_log.c ../src/ewf_string.c ../src/ewf_file.c test_ewf_hashtable.c */

int main()
{
	ewf_hashtable_t *table;
	char *test = NULL;
	char *test1 = "1";
	char *test2 = "2";
	char *test5 = "5";

	/* create a hash table of size 7 */
	if (ewf_hashtable_create(&table, 7) == EWF_ERROR)
	{
		printf("hashtable creation error\n");
	}
	else
	{
		printf("hashtable creation success\n");
	}

	/* insert an entry */
	if (ewf_hashtable_insert(&table, "test1", (void *) &test1) == EWF_ERROR)
	{
		printf("hashtable is full\n");
	}
	else
	{
		printf("insertion successfull\n");
	}

	/* insert another entry */
    if (ewf_hashtable_insert(&table, "test2", (void *) &test2) == EWF_ERROR)
    {
        printf("hashtable is full\n");
    }   
    else
    {
        printf("insertion successfull\n");
    }

	/* find an existing entry */
	if (ewf_hashtable_find(&table, "test1", (void *) &test) == EWF_ERROR)
	{
		printf("entry with key 'test1' not found\n");
	}
	else
	{
		printf("entry with key 'test1' found: '%s'\n", test);
	}

	/* try to find a non-existing entry */
	if (ewf_hashtable_find(&table, "test5", (void *) &test) == EWF_ERROR)
    {
        printf("entry with key 'test5' not found\n");
    }
    else
    {
		printf("entry with key 'test5' found: '%s'\n", test);
    }

	/* destroy the table */
	ewf_hashtable_destroy(&table);

	return 0;
}

