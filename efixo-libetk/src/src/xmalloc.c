
#include <stdlib.h>

#include <unistd.h>

#include <etk/log.h>

void *xmalloc(size_t size)
{
	void *ptr;

	trace("size:%zu", size);

	if (!(ptr = malloc(size))) {
		fatal("%s(%zu)", "malloc", size);
		exit(EXIT_FAILURE);
	}

	return ptr;
}
