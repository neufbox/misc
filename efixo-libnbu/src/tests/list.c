#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nbu/list.h"
#include "nbu/nbu.h"
#define FINI_DMALLOC 0
#include "dmalloc.h"

typedef struct s_test_t {
	char* string;
	int entier;
	char stringstat[50];
} test_t;

static int int_to_sort[] = {
	15,
	87,
	7,
	89,
	87,
	12,
	78,
	45,
	152,
	12,
	68,
	2,
	8,
	9,
	45,
	-1,
};

static int func_int_sort(void *d1, void *d2)
{
	return ((*(int*)d1) - (*(int*)d2));
}

void test_t_terminator(void* data)
{
	test_t* item = data;

	if(item->string != NULL)
	{
		free(item->string);
		item->string = NULL;
	}

	free(item);
}

int test_t_serialize(void* data, char** buf, size_t* buf_size)
{
	test_t* item = data;
	char* p = NULL;
	size_t size_string;
	
	if(item->string != NULL)
	{
		size_string = strlen(item->string) + 1;
	}
	else
	{
		/* Copy the NULL item */
		size_string = 1;
	}
	
	*buf_size = sizeof(test_t) - sizeof(char*) + size_string;

	*buf = calloc(1, *buf_size);

	p = *buf;
	
	memcpy(p, item->string, size_string);
	p += size_string;

	memcpy(p, &item->entier, sizeof(int));
	p += sizeof(int);

	memcpy(p, item->stringstat, sizeof(item->stringstat));
	p += sizeof(item->stringstat);
	
	return 0;
}

int test_t_unserialize(void** data, char* buf, size_t buf_size)
{
	test_t* item;
	char* p = NULL;
	size_t size_string = strlen(buf) + 1;

	NBU_UNUSED(buf_size);

	item = calloc(1, sizeof(test_t));

	p = buf;
	
	item->string = strdup(buf);
	p += size_string;
	
	item->entier = *(int*)p;
	p += sizeof(int);
	
	memcpy(&item->stringstat, p, sizeof(item->stringstat));
	
	*data = item;
	
	return 0;
}

int cmp(void* d1, void* d2)
{
	if(*(int*)d1 == *(int*)d2)
	{
		return 0;
	}
	else if(*(int*)d1 >= *(int*)d2)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int test_list(void)
{
	nbu_list_t* list = NULL;
	nbu_list_t* list2 = NULL;
	char* buf;
	size_t buf_len;
	
	int ret;
	int* i;
	int n;
	int c;
	
	test_t* item;
	
	unsigned long mark;

	/* get the current dmalloc position */
	mark = dmalloc_mark();
	/***********************************/
	
	list = nbu_list_new(sizeof(int));

	printf("ROOTNODE = %p\n", &(list->rootnode));
	
	/* insert_begin */
	printf("  * insert 20 elements in empty list (insert_begin)");
	for(n = 0; n < 20; n++)
	{
		i = calloc(1, sizeof(int));

		*i = n;
	
		nbu_list_insert_begin(list, i);
		
		printf(" %d %p\n", n, list->rootnode.next);
	}
	printf("  -- OK --\n");
	
	/* show */
	printf("  * show 20 elements\n     ");

	c = 0;
	nbu_list_iterator_start(list);
	
	while(nbu_list_iterator_is_valid(list))
	{
		void *data = nbu_list_get(list);
		if(data != NULL)
		{
			printf(" %d", *(int*)data);
		}
		else
		{
			printf("(it=%p) Error, data is NULL !\n",
			       list->iterator);
			break;
		}
		printf(" %p\n", list->iterator);

		nbu_list_iterator_next(list);

		c++;
	}
	if(c == 20)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}

	/* insert_end */
	printf("  * insert 20 elements (insert_end)");
	for(n = 0; n < 20; n++)
	{
		i = calloc(1, sizeof(int));

		*i = n;
	
		nbu_list_insert_end(list, i);
	}
	printf("  -- OK --\n");
	
	/* show */
	printf("  * show 40 elements\n     ");

	c = 0;
	nbu_list_iterator_start(list);
	while(nbu_list_iterator_is_valid(list))
	{
		printf(" %d", *(int*)nbu_list_get(list));
		
		nbu_list_iterator_next(list);

		c++;
	}
	if(c == 40)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}

	/* show (reverse)*/
	printf("  * show 40 elements (reverse)\n     ");

	c = 0;
	nbu_list_iterator_end(list);
	while(nbu_list_iterator_is_valid(list))
	{
		printf(" %d", *(int*)nbu_list_get(list));
		
		nbu_list_iterator_previous(list);

		c++;
	}
	if(c == 40)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}

	/* show (other method with has_next)*/
	printf("  * show 40 elements (test method has_next)\n     ");

	c = 0;
	nbu_list_iterator_start(list);
	if(nbu_list_iterator_is_valid(list))
	{
		do
		{
			if(c != 0)
			{
				nbu_list_iterator_next(list);
			}

			printf(" %d", *(int*)nbu_list_get(list));
			c++;
		} while(nbu_list_iterator_has_next(list));
	}
	if(c == 40)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}
	
	/* show (other method with has_previous)*/
	printf("  * show 40 elements (test method has_previous)\n     ");

	c = 0;
	nbu_list_iterator_end(list);
	if(nbu_list_iterator_is_valid(list))
	{
		do
		{
			if(c != 0)
			{
				nbu_list_iterator_previous(list);
			}

			printf(" %d", *(int*)nbu_list_get(list));
			c++;
		} while(nbu_list_iterator_has_previous(list));
	}
	if(c == 40)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}
	
	/* show (other method)*/
	printf("  * show 40 elements reverse (for method)\n     ");

	c = 0;
	for(nbu_list_iterator_end(list);
	    nbu_list_iterator_is_valid(list);
	    nbu_list_iterator_previous(list))
	{
		printf(" %d", *(int*)nbu_list_get(list));
		c++;
	}
	if(c == 40)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}
	
	/* Insert number 99 after first number 0 */
	printf("  * Insert number 99 after first number 0");
	i = calloc(1, sizeof(int));
	*i = 99;
	
	nbu_list_for_each(list)
	{
		if(*(int*)nbu_list_get(list) == 0)
		{
			nbu_list_insert_after(list, i);
			break;
		}
	}
	printf("  -- OK --\n");

	/* show rev */
	printf("  * show rev 41 elements\n     ");

	c = 0;
	
	nbu_list_for_each_data_rev(list, i, int)
	{
		printf(" %d", *i);
		c++;
	}
	if(c == 41)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}

	/* show */
	printf("  * show 41 elements\n     ");

	c = 0;
	
	nbu_list_for_each_data(list, i, int)
	{
		printf(" %d", *i);
		c++;
	}
	if(c == 41)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}
	
	/* contain */
	printf("  * test contain number 99");
	
	n = 99;
	if(nbu_list_contain(list, &n, cmp) == NBU_SUCCESS)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, contain failed ! --\n");
	}
	
	/* destroy node with number 0 */
	printf("  * remove item with number 0");
	
	c = 0;
	nbu_list_for_each_data(list, i, int)
	{
		if(*i == 0)
		{
			nbu_list_delete_backward(list);
			c++;
		}
	}
	if(c == 2)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, remove only %d element(s) --\n", c);
	}

	/* show */
	printf("  * show 39 elements\n     ");

	c = 0;
	
	nbu_list_for_each_data(list, i, int)
	{
		printf(" %d", *i);
		c++;
	}
	if(c == 39)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}
	
	/* serialiaze/unserialize */
	printf("  * testing serialize/unserialize and show list     \n");
	
	nbu_list_serialize(list, &buf, &buf_len, NULL);
	printf("           (ser)\n");

	nbu_list_unserialize(&list2, buf, buf_len, NULL, NULL);
	printf("           (unser)\n");

	free(buf);
	
	c = 0;
	
	printf("           (show)\n     ");
	
	nbu_list_for_each_data(list2, i, int)
	{
		printf(" %d", *i);
		c++;
	}
	if(c == 39)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, show only %d element(s) --\n", c);
	}
	
	/* destroy */
	printf("  * destroy unserialized list");

	nbu_list_destroy(list2);

	printf("  -- OK --\n");
	
	/* count control */
	printf("  * testing count    ");
	c = 0;
	c = nbu_list_count(list);

	if(c == 39)
	{
		printf("  -- OK --\n");
	}
	else
	{
		printf("  -- Error, count only %d element(s) --\n", c);	
	}

	/* destroy */
	printf("  * destroy list");

	nbu_list_destroy(list);

	printf("  -- OK --\n");
	
	printf("********* round 2, more complex list ! *********\n");

	printf("  * create list with terminator");

	list = nbu_list_new_with_terminator(sizeof(test_t), test_t_terminator);

	if(list != NULL)
	{
		printf("      -- OK --\n");
	}
	else
	{
		printf("      -- Error: nbu_list_new failed ! --\n");
		
		return 1;
	}
	
	printf("  * try to iterate with empty list");

	c = 0;
	nbu_list_for_each(list)
	{
		printf(" + ");
		c++;
	}
	if(c == 0)
	{
		printf("      -- OK --\n");
	}
	else
	{
		printf("      -- Error: empty list but one element at least is returned by nbu_list_for_each ! --\n");
	}
	
	/* insert one element */
	printf("  * insert one element");

	item = calloc(1, sizeof(test_t));
	
	item->string = strdup("Prout prout !");
	item->entier = 99;
	strncpy(item->stringstat, "Walalalalal", sizeof(item->stringstat) - 1);
	
	ret = nbu_list_insert_end(list, item);
	if(c == NBU_SUCCESS)
	{
		printf("      -- OK --\n");
	}
	else
	{
		printf("      -- Error: nbu_list_insert_end failed ! --\n");
	}

	/* show list */
	printf("  * show list (1 element)\n");
	
	c = 0;
	nbu_list_for_each_data(list, item, test_t)
	{
		printf("  (%s, %d, %s)", item->string, item->entier, item->stringstat);
		c++;
	}
	if(c == 1)
	{
		printf("      -- OK --\n");
	}
	else
	{
		printf("      -- Error: incorrect nbu_list_for_each ! --\n");
	}

	/* insert other element at start */
	printf("  * insert two elements");

	item = calloc(1, sizeof(test_t));
	
	item->string = strdup("Oh oh !");
	item->entier = 51;
	strncpy(item->stringstat, "Hiahahaha", sizeof(item->stringstat) - 1);
	
	ret = nbu_list_insert_begin(list, item);
	
	item = calloc(1, sizeof(test_t));

	item->string = strdup("Three Three !");
	item->entier = 207;
	strncpy(item->stringstat, "Padam Padam", sizeof(item->stringstat) - 1);
	
	ret = nbu_list_insert_begin(list, item) && (ret == NBU_SUCCESS);
	if(ret == NBU_SUCCESS)
	{
		printf("      -- OK --\n");
	}
	else
	{
		printf("      -- Error: nbu_list_insert_begin failed ! --\n");
	}
	
	/* show list */
	printf("  * show list (3 elements)\n");
	
	c = 0;
	nbu_list_for_each_data(list, item, test_t)
	{
		printf("  (%s, %d, %s)", item->string, item->entier, item->stringstat);
		c++;
	}
	if(c == 3)
	{
		printf("      -- OK --\n");
	}
	else
	{
		printf("      -- Error: incorrect nbu_list_for_each ! --\n");
	}
	
	/* serialize */
	printf("  * try to serialize\n");
	
	if(nbu_list_serialize(list, &buf, &buf_len, test_t_serialize) != NBU_SUCCESS)
	{
		printf("      -- Error: serialize failed ! --\n");
	}
	else
	{
		printf("      -- OK --\n");
	}

	/* unserialize */
	printf("  * try to unserialize\n");
	
	if(nbu_list_unserialize(&list2, buf, buf_len, test_t_unserialize ,test_t_terminator) != NBU_SUCCESS)
	{
		printf("      -- Error: unserialize failed ! --\n");
	}
	else
	{
		printf("      -- OK --\n");
	}

	free(buf);
	
	/* show list */
	printf("  * show unserialized list (3 elements)\n");
	
	c = 0;
	nbu_list_for_each_data(list2, item, test_t)
	{
		printf("  (%s, %d, %s)", item->string, item->entier, item->stringstat);
		c++;
	}
	if(c == 3)
	{
		printf("      -- OK --\n");
	}
	else
	{
		printf("      -- Error: incorrect nbu_list_for_each ! --\n");
	}
	
	/* destroy */
	printf("  * destroy unserialized list");
	
	nbu_list_destroy(list2);
	
	printf("      -- OK --\n");
	
	/* destroy */
	printf("  * destroy list");
	
	nbu_list_destroy(list);
	
	printf("      -- OK --\n");
	
	list = nbu_list_new(sizeof(int));

	/* insert_begin */
	printf("  * insert 20 elements in empty list (insert_end)");
	for(n = 0; n < 20; n++)
	{
		i = calloc(1, sizeof(int));

		*i = n;
	
		nbu_list_insert_end(list, i);
	}
	printf("  -- OK --\n");

	/* random delete */
	n = 0;
	printf("  * remove 10 elements (odd index) (delete_backward)\n");
	nbu_list_for_each_data(list, i, int)
	{
		if(n % 2 == 0)
		{
			printf(" Remove %d th. element (data = %d)\n", n, *(int*)nbu_list_get(list));
			nbu_list_delete_backward(list);
		}
		n++;
	}
	
	printf("  -- OK --\n");
	/* show */
	printf("  * show 10 elements\n");
	nbu_list_for_each_data(list, i, int)
	{
		printf(" %d", *(int*)nbu_list_get(list));
	}
	printf("  -- OK --\n");
	
	nbu_list_destroy(list);

	/* sort test */
	list = nbu_list_new(sizeof(int));
	
	/* insert_begin */
	printf("  * insert XX elements to sort in empty list (insert_end)");
	n = 0;
	
	while(int_to_sort[n] != -1)
	{
		i = calloc(1, sizeof(int));

		*i = int_to_sort[n];
	
		nbu_list_insert_end(list, i);

		n++;
	}
	printf("  -- OK --\n");
	
	/* show */
	printf("  * show XX elements\n     ");

	c = 0;
	nbu_list_iterator_start(list);
	
	while(nbu_list_iterator_is_valid(list))
	{
		void *data = nbu_list_get(list);
		if(data != NULL)
		{
			printf(" %d", *(int*)data);
		}
		else
		{
			printf("(it=%p) Error, data is NULL !\n",
			       list->iterator);
			break;
		}
		printf(" %p\n", list->iterator);

		nbu_list_iterator_next(list);

		c++;
	}
	printf("  -- OK ??? --\n");
	
	/* sort */
	printf("  * sort XX elements\n     ");

	nbu_list_sort(&list, func_int_sort);
	
	printf("  -- OK ??? --\n");
	
	/* show */
	printf("  * show XX elements sorted !\n     ");

	c = 0;
	nbu_list_iterator_start(list);
	
	while(nbu_list_iterator_is_valid(list))
	{
		void *data = nbu_list_get(list);
		if(data != NULL)
		{
			printf(" %d", *(int*)data);
		}
		else
		{
			printf("(it=%p) Error, data is NULL !\n",
			       list->iterator);
			break;
		}
		printf(" %p\n", list->iterator);

		nbu_list_iterator_next(list);

		c++;
	}
	printf("  -- OK ???? --\n");
	
	/* destroy */
	nbu_list_destroy(list);
	
	/*
	 * log unfreed pointers that have been added to
	 * the heap since mark
	 */
	dmalloc_log_changed(mark,
			    1 /* log unfreed pointers */,
			    0 /* do not log freed pointers */,
			    1 /* log each pnt otherwise summary */);
	
	
	/***********************************************************/
	
	return 0;
}

static void __attribute__((constructor)) __init(void)
{
	dmalloc_debug_setup("log-non-free,check-fence,log=/tmp/nbu.%p");
}

int main(void)
{
	return test_list();
}

static void __attribute__((destructor)) __fini(void)
{
	dmalloc_shutdown();
}
