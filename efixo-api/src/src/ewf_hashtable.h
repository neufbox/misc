#ifndef _EWF_HASHTABLE_H_
#define _EWF_HASHTABLE_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <search.h>

/*!
 * \file ewf_hastable.h
 * \brief Functions dealing with hash tables.
 * \author Raphael HUCK
 */

/*! Structure representing a hash table.
 */
typedef struct hsearch_data ewf_hashtable_t;

/*! Create a hash table.
 * \note This function will take care of allocating memory for the hash table so you don't need to.
 * \param htab pointer to return the created hash table
 * \param size size (in number of entries) of the hash table to create
 * \return
 * - \b EWF_SUCCESS if the hash table was successfully created
 * - \b EWF_ERROR otherwise
 */
int ewf_hashtable_create(ewf_hashtable_t **htab, size_t size);

/*! Destroy a hash table.
 * \note This function will take care of freeing the memory for the hash table so you don't need to.
 * \param htab pointer to the hash table to destroy
 * \return
 * - \b EWF_SUCCESS if the hash table was successfully destroyed
 * - \b EWF_ERROR otherwise
 */
int ewf_hashtable_destroy(ewf_hashtable_t **htab);

/*! Insert an entry in the hash table.
 * \param htab pointer to the hash table to insert into
 * \param key key to identify the entry to insert
 * \param data pointer to the entry to insert
 * \return
 * - \b EWF_SUCCESS if the entry was successfully inserted
 * - \b EWF_ERROR otherwise
 */
int ewf_hashtable_insert(ewf_hashtable_t **htab, const char *key, void **data);

/*! Find an entry in the hash table.
 * \param htab pointer to the hash table where to find the entry
 * \param key key of the entry to find
 * \param data pointer to return the found entry
 * \return
 * - \b EWF_SUCCESS if the entry was successfully found
 * - \b EWF_ERROR otherwise
 */
int ewf_hashtable_find(ewf_hashtable_t **htab, const char *key, void **data);

#endif /* _EWF_HASHTABLE_H_ */

