/*
 * A generic kernel FIFO implementation.
 *
 * Copyright (C) 2010 Tanguy Bouzéloc <tanguy.bouzeloc@efixo.com>
 * Copyright (C) 2009 Stefani Seibold <stefani@seibold.net>
 * Copyright (C) 2004 Stelian Pop <stelian@popies.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <etk/fifo.h>
#include <etk/math.h>
#include <etk/log.h>
#include <etk/common.h>

static void _fifo_init(struct fifo *fifo, void *buffer, unsigned int size)
{
	fifo->buffer = buffer;
	fifo->size = size;

	fifo_reset(fifo);
}

/**
 * fifo_init - initialize a FIFO using a preallocated buffer
 * @fifo: the fifo to assign the buffer
 * @buffer: the preallocated buffer to be used.
 * @size: the size of the internal buffer, this has to be a power of 2.
 *
 */
void fifo_init(struct fifo *fifo, void *buffer, unsigned int size)
{
	/* size must be a power of 2 */
	if (!is_power_of_2(size)) {
		err("fifo size is not a power of two");
		return;
	}

	_fifo_init(fifo, buffer, size);
}

/**
 * fifo_alloc - allocates a new FIFO internal buffer
 * @fifo: the fifo to assign then new buffer
 * @size: the size of the buffer to be allocated, this have to be a power of 2.
 *
 * This function dynamically allocates a new fifo internal buffer
 *
 * The size will be rounded-up to a power of 2.
 * The buffer will be release with fifo_free().
 * Return 0 if no error, otherwise the an error code
 */
int fifo_alloc(struct fifo *fifo, unsigned int size)
{
	unsigned char *buffer;

	/*
	 * round up to the next power of 2, since our 'let the indices
	 * wrap' technique works only in this case.
	 */
	if (!is_power_of_2(size)) {
		size = roundup_pow_of_two(size);
	}

	buffer = malloc(size);
	if (!buffer) {
		_fifo_init(fifo, NULL, 0);
		err("out of memory");
		return -1;
	}

	_fifo_init(fifo, buffer, size);

	return 0;
}

/**
 * fifo_free - frees the FIFO internal buffer
 * @fifo: the fifo to be freed.
 */
void fifo_free(struct fifo *fifo)
{
	free(fifo->buffer);
	_fifo_init(fifo, NULL, 0);
}

/**
 * fifo_skip - skip output data
 * @fifo: the fifo to be used.
 * @len: number of bytes to skip
 */
void fifo_skip(struct fifo *fifo, unsigned int len)
{
	if (len < fifo_len(fifo)) {
		__fifo_add_out(fifo, len);
		return;
	}
	fifo_reset_out(fifo);
}

static inline void __fifo_in_data(struct fifo *fifo,
				  const void *from, unsigned int len,
				  unsigned int off)
{
	unsigned int l;

	/*
	 * Ensure that we sample the fifo->out index -before- we
	 * start putting bytes into the fifo.
	 */

	off = __fifo_off(fifo, fifo->in + off);

	/* first put the data starting from fifo->in to buffer end */
	l = min(len, fifo->size - off);
	memcpy(fifo->buffer + off, from, l);

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(fifo->buffer, from + l, len - l);
}

static inline void __fifo_out_data(struct fifo *fifo,
				   void *to, unsigned int len, unsigned int off)
{
	unsigned int l;

	/*
	 * Ensure that we sample the fifo->in index -before- we
	 * start removing bytes from the fifo.
	 */

	off = __fifo_off(fifo, fifo->out + off);

	/* first get the data from fifo->out until the end of the buffer */
	l = min(len, fifo->size - off);
	memcpy(to, fifo->buffer + off, l);

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy(to + l, fifo->buffer, len - l);
}

unsigned int __fifo_in_n(struct fifo *fifo,
			 const void *from, unsigned int len,
			 unsigned int recsize)
{
	if (fifo_avail(fifo) < len + recsize)
		return len + 1;

	__fifo_in_data(fifo, from, len, recsize);
	return 0;
}

/**
 * fifo_in - puts some data into the FIFO
 * @fifo: the fifo to be used.
 * @from: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most @len bytes from the @from buffer into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
unsigned int fifo_in(struct fifo *fifo, const void *from, unsigned int len)
{
	len = min(fifo_avail(fifo), len);

	__fifo_in_data(fifo, from, len, 0);
	__fifo_add_in(fifo, len);
	return len;
}

unsigned int __fifo_in_generic(struct fifo *fifo,
			       const void *from, unsigned int len,
			       unsigned int recsize)
{
	return __fifo_in_rec(fifo, from, len, recsize);
}

unsigned int __fifo_out_n(struct fifo *fifo,
			  void *to, unsigned int len, unsigned int recsize)
{
	if (fifo_len(fifo) < len + recsize)
		return len;

	__fifo_out_data(fifo, to, len, recsize);
	__fifo_add_out(fifo, len + recsize);
	return 0;
}

/**
 * fifo_out - gets some data from the FIFO
 * @fifo: the fifo to be used.
 * @to: where the data must be copied.
 * @len: the size of the destination buffer.
 *
 * This function copies at most @len bytes from the FIFO into the
 * @to buffer and returns the number of copied bytes.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
unsigned int fifo_out(struct fifo *fifo, void *to, unsigned int len)
{
	len = min(fifo_len(fifo), len);

	__fifo_out_data(fifo, to, len, 0);
	__fifo_add_out(fifo, len);

	return len;
}

/**
 * fifo_out_peek - copy some data from the FIFO, but do not remove it
 * @fifo: the fifo to be used.
 * @to: where the data must be copied.
 * @len: the size of the destination buffer.
 * @offset: offset into the fifo
 *
 * This function copies at most @len bytes at @offset from the FIFO
 * into the @to buffer and returns the number of copied bytes.
 * The data is not removed from the FIFO.
 */
unsigned int fifo_out_peek(struct fifo *fifo, void *to, unsigned int len,
			   unsigned offset)
{
	len = min(fifo_len(fifo), len + offset);

	__fifo_out_data(fifo, to, len, offset);
	return len;
}

unsigned int __fifo_out_generic(struct fifo *fifo,
				void *to, unsigned int len,
				unsigned int recsize, unsigned int *total)
{
	return __fifo_out_rec(fifo, to, len, recsize, total);
}

unsigned int __fifo_peek_generic(struct fifo *fifo, unsigned int recsize)
{
	if (recsize == 0)
		return fifo_avail(fifo);

	return __fifo_peek_n(fifo, recsize);
}

void __fifo_skip_generic(struct fifo *fifo, unsigned int recsize)
{
	__fifo_skip_rec(fifo, recsize);
}

size_t fifo_serialize(struct fifo_serialized **fifo_ser, struct fifo *fifo)
{
	size_t len = sizeof(struct fifo_serialized) + fifo->size;
	*fifo_ser = (struct fifo_serialized *)malloc(len);

	if (*fifo_ser == NULL) {
		err("out of memory");
		return 0;
	}

	(*fifo_ser)->size = fifo->size;
	(*fifo_ser)->in = fifo->in;
	(*fifo_ser)->out = fifo->out;
	memcpy((*fifo_ser)->buffer, fifo->buffer, fifo->size);
	return len;
}

int fifo_unserialize(struct fifo *fifo, struct fifo_serialized *fifo_ser)
{
	if (fifo_alloc(fifo, fifo_ser->size))
		return -1;

	fifo->size = fifo_ser->size;
	fifo->in = fifo_ser->in;
	fifo->out = fifo_ser->out;

	memcpy(fifo->buffer, fifo_ser->buffer, fifo_ser->size);
	return 0;
}
