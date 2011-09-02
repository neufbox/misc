#ifndef _LIST_HEAD_DICT_H_
#define _LIST_HEAD_DICT_H_

#include "etk/list.h"

/*
 * An user friendly dictionnary based on list_head
 */

struct dict {
        struct dict_entry *entry;
        struct list_head head;
};

struct dict_entry {
        char *key;
        char *data;
        struct list_head list;
};

void dict_init(struct dict *dict);
void dict_free(struct dict *dict);

void dict_put(struct dict *dict,
              const char *key,
              const char *data);

int dict_remove(struct dict *dict,
                const char *key);

#define dict_for_each_entry(key, data, dict)                            \
        for ((dict)->entry = list_entry((dict)->head.next, typeof(*((dict)->entry)), list), \
                     key = ((dict)->entry)->key, data = ((dict)->entry)->data; \
             &((dict)->entry->list) != &((dict)->head);                 \
             (dict)->entry = list_entry((dict)->entry->list.next, typeof(*((dict)->entry)), list), \
                     key = ((dict)->entry)->key, data = ((dict)->entry)->data)

#endif
