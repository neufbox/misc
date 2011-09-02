#ifndef _PLUGIN_H_
#define _PLUGIN_H_ 1

#include <etk/list.h>

struct plugin {
	struct list_head list;
	char const *name;
	int fd;
	int (*init) (struct plugin *);
	void (*cleanup) (struct plugin *);
	int (*poll) (struct plugin *);
	int (*event) (struct plugin *);
};

#endif
