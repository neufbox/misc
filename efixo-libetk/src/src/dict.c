#include <stdlib.h>
#include <string.h>

#include "etk/dict.h"

void dict_init(struct dict *dict)
{
	INIT_LIST_HEAD(&(dict->head));
}

void dict_free(struct dict *dict)
{
	struct dict_entry *entry_safe;

	list_for_each_entry_safe(dict->entry, entry_safe, &(dict->head), list) {
		list_del(&(dict->entry->list));
		free(dict->entry->key);
		free(dict->entry->data);
		free(dict->entry);
		dict->entry = NULL;
	}
}

void dict_put(struct dict *dict, const char *key, const char *data)
{
	list_for_each_entry(dict->entry, &(dict->head), list) {
		if (strcmp(dict->entry->key, key) == 0) {
			/* always here */
			free(dict->entry->data);
			dict->entry->data = strdup(data);
			return;
		}
	}

	/* new entry */
	dict->entry = calloc(1, sizeof(struct dict_entry));

	dict->entry->key = strdup(key);
	dict->entry->data = strdup(data);

	list_add_tail(&(dict->entry->list), &(dict->head));
}

int dict_remove(struct dict *dict, const char *key)
{
	int ret = -1;
	struct dict_entry *entry_safe;

	list_for_each_entry_safe(dict->entry, entry_safe, &(dict->head), list) {
		if (strcmp(dict->entry->key, key) == 0) {
			ret = 0;

			list_del(&(dict->entry->list));
			free(dict->entry->key);
			free(dict->entry->data);
			free(dict->entry);
			dict->entry = NULL;

			break;
		}
	}

	return ret;
}
