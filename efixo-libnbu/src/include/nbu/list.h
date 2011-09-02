#ifndef _NBU_LIST_H_
#define _NBU_LIST_H_

#include <stdio.h>

typedef struct s_nbu_list_t nbu_list_t;
typedef struct s_nbu_list_node_t nbu_list_node_t;

struct s_nbu_list_node_t {
	nbu_list_node_t* prev;
	nbu_list_node_t* next;
	void* data;
};

/* NB: cyclic list, rootnode is use to tag the start/end of the list */
struct s_nbu_list_t {
	size_t count;
	size_t data_size;
	void (*data_terminator_func)(void *);
	nbu_list_node_t rootnode;
	nbu_list_node_t *iterator;
};

/*
 * return an nbu_list_t pointer to a new empty list of data_size node.
 * data_size is only used in serialize function if you don't set your custom
 * serialize function with nbu_list_set_ser_funcs (In which case, put the size 
 * of the structure)
 */
nbu_list_t* nbu_list_new(size_t data_size);

/*
 * return an nbu_list_t pointer to a new empty list with terminator func
 * for free data node
 */
nbu_list_t* nbu_list_new_with_terminator(size_t data_size,
					 void (*data_terminator_func)(void *));

/*
 * Destroy list and remove/free (with terminator function if exist) all nodes
 */
void nbu_list_destroy(nbu_list_t* list);

/*
 * Return list node count
 */
size_t nbu_list_count(nbu_list_t* list);

/*
 * Move iterator
 */
void nbu_list_iterator_start(nbu_list_t* list);
void nbu_list_iterator_end(nbu_list_t* list);
void nbu_list_iterator_next(nbu_list_t* list);
void nbu_list_iterator_previous(nbu_list_t* list);

/*
 * Test iterator
 */
int nbu_list_iterator_is_valid(nbu_list_t* list);
int nbu_list_iterator_is_last(nbu_list_t* list);
int nbu_list_iterator_is_first(nbu_list_t* list);
int nbu_list_iterator_has_next(nbu_list_t* list);
int nbu_list_iterator_has_previous(nbu_list_t* list);

/*******************/
/* iterator depend */
/*******************/

/*
 * Return data pointed by iterator
 */
void* nbu_list_get(nbu_list_t* list);

/*
 * Insert new node with this data after iterator
 */
int nbu_list_insert_after(nbu_list_t* list,
			  void* data);

/*
 * Insert new node with this data before iterator
 */
int nbu_list_insert_before(nbu_list_t* list,
			    void* data);

/*
 * Delete node and free data (with terminator func if exist) and move
 * iterator forward
 */
void nbu_list_delete_forward(nbu_list_t* list);

/*
 * Delete node and free data (with terminator func if exist) and move
 * iterator backward
 */
void nbu_list_delete_backward(nbu_list_t* list);

/**********************/
/* No iterator depend */
/**********************/

/*
 * Insert new node with this data at start of list
 */
int nbu_list_insert_begin(nbu_list_t* list,
			  void* data);

/*
 * Insert new node with this data at end of list
 */
int nbu_list_insert_end(nbu_list_t* list,
			void* data);

/*
 * Delete the first node of list and free (with terminator func if exist) data
 */
void nbu_list_delete_begin(nbu_list_t* list);

/*
 * Delete the last node of list and free (with terminator func if exist) data
 */
void nbu_list_delete_end(nbu_list_t* list);

/*
 * Get data pointer with index. Return NULL on error
 */
void *nbu_list_get_at_index(nbu_list_t* list,
			    size_t idx);

/*
 * Return 1 if list contain this data, otherwise 0
 */
int nbu_list_contain(nbu_list_t* list,
		     void* data,
		     int (*compare) (void *, void *));

/*
 * Serialize list and return an buffer (think to free it)
 */
int nbu_list_serialize(nbu_list_t* list, 
		       char** buf, 
		       size_t* buf_len,
		       int (*data_serialize_func)(void *, char **, size_t*));

/*
 * Unserialize buffer to a new list (think to nbu_list_destroy this new list 
 * at end)
 */
int nbu_list_unserialize(nbu_list_t** list,
			 char* buf,
			 size_t buf_len,
			 int (*data_unserialize_func)(void **, char *, size_t),
			 void (*data_terminator_func)(void *));

/*
 * sort list.
 * The sort_func give by user is used to compare two elements like strcmp:
 *       must return >0 for a after b
 *       must return <0 for a before b
 *       else 0
 */
int nbu_list_sort(nbu_list_t** list,
		  int (*sort_func)(void *, void *));

/*
 * Macro
 */
#define nbu_list_for_each(list) \
	for(nbu_list_iterator_start(list); \
	    nbu_list_iterator_is_valid(list); \
	    nbu_list_iterator_next(list))

#define nbu_list_for_each_rev(list)	 \
	for(nbu_list_iterator_end(list); \
	    nbu_list_iterator_is_valid(list); \
	    nbu_list_iterator_previous(list))

#define nbu_list_for_each_data(list, data, datatype) \
	for(nbu_list_iterator_start(list);	 \
	    nbu_list_iterator_is_valid(list) && (void*)(data = (datatype *)nbu_list_get(list)) != list; \
	    nbu_list_iterator_next(list))

#define nbu_list_for_each_data_rev(list, data, datatype) \
	for(nbu_list_iterator_end(list); \
	    nbu_list_iterator_is_valid(list) && (void*)(data = (datatype *)nbu_list_get(list)) != list;	\
	    nbu_list_iterator_previous(list))

#endif /* _NBU_LIST_H_ */
