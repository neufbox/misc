#include "nbu/list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "nbu/nbu.h"

static void nbu_list_destroy_node(nbu_list_t* list,
				  nbu_list_node_t* node_ref)
{
	assert(list != NULL);
	assert(node_ref != NULL);
	
	if(list->data_terminator_func != NULL)
	{
		list->data_terminator_func(node_ref->data);
	}
	else
	{
		free(node_ref->data);
	}
	node_ref->data = NULL;

	if(node_ref->next != NULL)
	{
		node_ref->next->prev = node_ref->prev;
	}
	else
	{
		/* end node of list */
		list->rootnode.prev = node_ref->prev;
	}

	if(node_ref->prev != NULL)
	{
		node_ref->prev->next = node_ref->next;
	}
	else
	{
		/* head node of list */
		list->rootnode.next = node_ref->next;
	}
	
	free(node_ref);
	
	list->count--;
}

static int nbu_list_insert(nbu_list_t* list,
			   void* data,
			   nbu_list_node_t* node_ref,
			   int direction)
{
	nbu_list_node_t* node = NULL;
	int ret = NBU_ERROR;
	
	assert(list != NULL);

	node = calloc(1, sizeof(nbu_list_node_t));
       
	/* printf("[=====+ %p, node_ref=%p, data=%p +=====]\n", (void*)node, (void*)node_ref, data); */

	if(node != NULL)
	{
		node->data = data;
		
		if(node_ref == NULL)
		{
			if(list->rootnode.next == NULL && list->rootnode.prev == NULL)
			{
				/* first insertion */;
				list->rootnode.next = node;
				list->rootnode.prev = node;

				node->prev = &(list->rootnode);
				node->next = &(list->rootnode);
				
				ret = NBU_SUCCESS;
				list->count++;
			}
			else
			{
				/* Error ! */
				free(node);

				/* printf("Insertion failed, node_ref == NULL but list is not empty !\n"); */
		
				ret = NBU_ERROR;
			}
		}
		else
		{
			if(direction)
			{
				node->prev = node_ref;
				node->next = node_ref->next;
				
				if(node->next == NULL)
				{
					/* last item */
					list->rootnode.prev = node;
				}
				
				if(node_ref->next != NULL)
				{
					node_ref->next->prev = node;
				}
				
				node_ref->next = node;
				
			}
			else
			{
				node->prev = node_ref->prev;
				node->next = node_ref;
				
				if(node->prev == NULL)
				{
					/* first item */
					list->rootnode.next = node;
				}
				
				if(node_ref->prev != NULL)
				{
					node_ref->prev->next = node;
				}
				node_ref->prev = node;
				
			}
			
			list->count++;
			ret = NBU_SUCCESS;
		}
	
		/* if(ret == NBU_SUCCESS) */
/* 		{ */
/* 			printf("(Ins) node=%p, prev=%p, next=%p | lhead=%p, lfoot=%p\n",  */
/* 			       (void*)node, */
/* 			       (void*)node->prev, */
/* 			       (void*)node->next, */
/* 			       (void*)list->rootnode.next, */
/* 			       (void*)list->rootnode.prev); */
/* 		} */

	}

	return ret;
}

nbu_list_t* nbu_list_new(size_t data_size)
{
	nbu_list_t* list = calloc(1, sizeof(nbu_list_t));
	if(list != NULL)
	{
		list->data_size = data_size;
	}
	
	return list;
}

nbu_list_t* nbu_list_new_with_terminator(size_t data_size,
					 void (*data_terminator_func)(void *))
{
	nbu_list_t* list = nbu_list_new(data_size);
	if(list != NULL)
	{
		list->data_terminator_func = data_terminator_func;
	}

	return list;
}

void nbu_list_destroy(nbu_list_t* list)
{
	nbu_list_node_t* next_node;
	
	if(list == NULL)
	{
		return;
	}

	nbu_list_iterator_start(list);
	while(nbu_list_iterator_is_valid(list))
	{
		next_node = list->iterator->next;
		
		nbu_list_destroy_node(list, list->iterator);
		
		list->iterator = next_node;
	}

	free(list);
}

size_t nbu_list_count(nbu_list_t* list)
{
	assert(list != NULL);

	return list->count;
}

void nbu_list_iterator_start(nbu_list_t* list)
{
	assert(list != NULL);

	list->iterator = list->rootnode.next;
}

void nbu_list_iterator_end(nbu_list_t* list)
{
	assert(list != NULL);
	
	list->iterator = list->rootnode.prev;
}

void nbu_list_iterator_next(nbu_list_t* list)
{
	assert(list != NULL);
	assert(list->iterator != NULL);
	
	list->iterator = list->iterator->next;
}

void nbu_list_iterator_previous(nbu_list_t* list)
{
	assert(list != NULL);
	assert(list->iterator != NULL);

	list->iterator = list->iterator->prev;
}

int nbu_list_iterator_is_valid(nbu_list_t* list)
{
	return (list->iterator != NULL && list->iterator != &(list->rootnode));
}

int nbu_list_iterator_is_last(nbu_list_t* list)
{
	return (list->iterator == list->rootnode.prev);
}

int nbu_list_iterator_is_first(nbu_list_t* list)
{
	return (list->iterator == list->rootnode.next);
}

int nbu_list_iterator_has_next(nbu_list_t* list)
{
	return (list->iterator->next != NULL && list->iterator->next != &(list->rootnode));
}

int nbu_list_iterator_has_previous(nbu_list_t* list)
{
	return (list->iterator->prev != NULL && list->iterator->prev != &(list->rootnode));
}

void* nbu_list_get(nbu_list_t* list)
{
	assert(list != NULL);
	assert(list->iterator != NULL);
	
	return list->iterator->data;
}

int nbu_list_insert_after(nbu_list_t* list,
			  void* data)
{
	return nbu_list_insert(list, data, list->iterator, 1);
}

int nbu_list_insert_before(nbu_list_t* list,
			   void* data)
{
	return nbu_list_insert(list, data, list->iterator, 0);
}

void nbu_list_delete_forward(nbu_list_t* list)
{
	nbu_list_node_t* node_next;
	
	assert(list != NULL);
	assert(list->iterator != NULL);
	
	node_next = list->iterator->next;

	nbu_list_destroy_node(list, list->iterator);

	list->iterator = node_next;
}

void nbu_list_delete_backward(nbu_list_t* list)
{
	nbu_list_node_t* node_prev;
	
	assert(list != NULL);
	assert(list->iterator != NULL);
	
	node_prev = list->iterator->prev;

	nbu_list_destroy_node(list, list->iterator);
	
	list->iterator = node_prev;
}

int nbu_list_insert_begin(nbu_list_t* list,
			  void* data)
{
	return nbu_list_insert(list, data, list->rootnode.next, 0);
}

int nbu_list_insert_end(nbu_list_t* list,
			void* data)
{
	return nbu_list_insert(list, data, list->rootnode.prev, 1);
}

void nbu_list_delete_begin(nbu_list_t* list)
{
	nbu_list_destroy_node(list, list->rootnode.next);
}

void nbu_list_delete_end(nbu_list_t* list)
{
	nbu_list_destroy_node(list, list->rootnode.prev);
}

void *nbu_list_get_at_index(nbu_list_t* list,
			    size_t ind)
{
	void *data = NULL; 
	size_t i;
	
	if(list == NULL)
	{
		return NULL;
	}

	if(ind >= nbu_list_count(list))
	{
		return NULL;
	}
	
	i = 0;
	nbu_list_for_each_data(list, data, void)
	{
		if(i == ind)
		{
			break;
		}

		i++;
	}

	return data;
}

int nbu_list_contain(nbu_list_t* list,
		     void* data,
		     int (*compare)(void *, void *))
{
	nbu_list_node_t* marker = list->rootnode.next;
	int ret = NBU_ERROR;

	while(marker != NULL && marker != &(list->rootnode))
	{
		if(compare(marker->data, data) == 0)
		{
			ret = NBU_SUCCESS;
			break;
		}
		marker = marker->next;
	}

	return ret;
}

int nbu_list_serialize(nbu_list_t* list, 
		       char** buf, 
		       size_t* buf_len,
		       int (*data_serialize_func)(void *, char **, size_t*))
{
	char* buf_tmp = NULL;
	char* item_buf = NULL;
	char* p = NULL;

	size_t item_buf_size;
	int b_alloc = 0;
	int b_used = 0;
	int b_need = 0;
	
	int custom_item_ser = (data_serialize_func != NULL);
	nbu_list_node_t* marker = list->rootnode.next;
	
	/* alloc for list with static item */
	b_alloc = sizeof(nbu_list_t) + ((sizeof(size_t) + list->data_size) * list->count);
	*buf = malloc(b_alloc);
	if(*buf == NULL)
	{
		return NBU_ERROR;
	}

	p = *buf;
	
	memcpy(p, list, sizeof(nbu_list_t));
	
	((nbu_list_t *)p)->data_terminator_func = NULL;
	((nbu_list_t *)p)->iterator = NULL;
	((nbu_list_t *)p)->rootnode.next = NULL;
	((nbu_list_t *)p)->rootnode.prev = NULL;

	b_used = sizeof(nbu_list_t);
	
	p += b_used;
	
	while(marker != NULL && marker != &(list->rootnode))
	{
		if(custom_item_ser)
		{
			if(data_serialize_func(marker->data, &item_buf, &item_buf_size) != 0)
			{
				/* unforgetable error :s */
				free(*buf);
				*buf = NULL;
				return NBU_ERROR;
			}
		}
		else
		{
			/* get only struct of item */
			item_buf = marker->data;
			item_buf_size = list->data_size;
		}
		
		b_need = sizeof(size_t) + item_buf_size;
		if(b_used + b_need > b_alloc)
		{
			/* need more space */
			buf_tmp = NULL;
			buf_tmp = realloc(*buf, b_used + b_need);
			if(buf_tmp)
			{
				*buf = buf_tmp;
			}
			else
			{
				/* error */
				free(*buf);
				*buf = NULL;
				return NBU_ERROR;
			}
			
			/* resynchro */
			p = *buf + b_used;
			b_alloc = b_used + b_need;
		}
		
		/* copy size item */
		memcpy(p, &item_buf_size, sizeof(size_t));
		p += sizeof(size_t);
		
		/* copy item */
		memcpy(p, item_buf, item_buf_size);
		if(custom_item_ser)
		{
			free(item_buf);
		}
		p += item_buf_size;
		
		/* refresh values */
		b_used += b_need;
		
		/* go next */
		marker = marker->next;
	}

	*buf_len = b_alloc;

	return NBU_SUCCESS;
}

int nbu_list_unserialize(nbu_list_t** list,
			 char* buf,
			 size_t buf_len,
			 int (*data_unserialize_func)(void **, char *, size_t),
			 void (*data_terminator_func)(void *))
{
	unsigned int i;
	size_t count = 0;
	char* p = NULL;
	void* item = NULL;
	size_t item_size;
	
	/* todo, use buf_len to control buffer out ?*/
	(void)(buf_len);
	
	*list = calloc(1, sizeof(nbu_list_t));
	memcpy(*list, buf, sizeof(nbu_list_t));
	
	(*list)->data_terminator_func = data_terminator_func;
	count = (*list)->count;
	(*list)->count = 0;
	
	p = buf + sizeof(nbu_list_t);
	
	for(i = 0; i < count; i++)
	{
		memcpy(&item_size, p, sizeof(item_size));
		
		p += sizeof(size_t);
		
		if(data_unserialize_func != NULL)
		{
			if(data_unserialize_func(&item, p, item_size) != 0)
			{
				/* oups ! */
				
				/* free alloc struct */
				nbu_list_destroy(*list);
				
				*list = NULL;
				
				return NBU_ERROR;
			}
		}
		else
		{
			/* alloc item_size bytes and copy item in it */
			item = calloc(1, item_size);
			memcpy(item, p, item_size);
		}
		
		nbu_list_insert_end(*list, item);
		
		p += item_size;
	}
	
	return NBU_SUCCESS;
}

static void _nbu_list_void_terminator(void *data)
{
	(void)(data); /* do nothing !*/
	
	return;
}

int nbu_list_sort(nbu_list_t** list,
		  int (*sort_func)(void *, void *))
{
	void *data = NULL, *data_sorted = NULL;
	int inserted = 0, res = 0;
	nbu_list_t *list_sorted = NULL, *list_tosort = NULL;
	
	list_tosort = *list;
	
	assert(list_tosort != NULL);

	list_tosort->data_terminator_func = _nbu_list_void_terminator;

	list_sorted = nbu_list_new(list_tosort->data_size);
	list_sorted->data_terminator_func = list_tosort->data_terminator_func;
	
	nbu_list_for_each_data(list_tosort, data, void)
	{
		nbu_list_for_each_data(list_sorted, data_sorted, void)
		{
			res = sort_func(data, data_sorted);
			if(res > 0)
			{
				continue;
			}
			else
			{
				nbu_list_insert_before(list_sorted, data);
				inserted = 1;
				break;
			}
		}
			
		if(!inserted)
		{
			nbu_list_insert_end(list_sorted, data);
		}

		inserted = 0;
	}

	*list = list_sorted;
	
	nbu_list_destroy(list_tosort);
	
	return NBU_SUCCESS;
}
