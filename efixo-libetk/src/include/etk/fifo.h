/*
 * A generic FIFO implementation.
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

#ifndef _FIFO_H_
#define _FIFO_H_
#include <stdbool.h>

struct fifo {
	unsigned char *buffer;	/* the buffer holding the data */
	unsigned int size;	/* the size of the allocated buffer */
	unsigned int in;	/* data is added at offset (in % size) */
	unsigned int out;	/* data is extracted from off. (out % size) */
};

struct fifo_serialized {
	unsigned int size;
	unsigned int in;
	unsigned int out;
	unsigned char buffer[0];
};

/*
 * Macros for declaration and initialization of the fifo datatype
 */

/* helper macro */
#define __fifo_initializer(s, b) \
	{ \
		.size	= s, \
		.in	= 0, \
		.out	= 0, \
		.buffer = b \
	}

/**
 * DEFINE_FIFO - macro to define and initialize a fifo
 * @name: name of the declared fifo datatype
 * @size: size of the fifo buffer. Must be a power of two.
 *
 * Note1: the macro can be used for global and local fifo data type variables
 * Note2: the macro creates two objects:
 *  A fifo object with the given name and a buffer for the fifo
 *  object named name##fifo_buffer
 */
#define DEFINE_FIFO(name, size) \
	unsigned char name##fifo_buffer[size]; \
	struct fifo name = __fifo_initializer(size, name##fifo_buffer)

#define DEFINE_STATIC_FIFO(name, size) \
	static unsigned char name##fifo_buffer[size]; \
	static struct fifo name = __fifo_initializer(size, name##fifo_buffer)

extern void fifo_init(struct fifo *fifo, void *buffer, unsigned int size);
extern int fifo_alloc(struct fifo *fifo, unsigned int size);
extern void fifo_free(struct fifo *fifo);
extern unsigned int fifo_in(struct fifo *fifo,
			    const void *from, unsigned int len);
extern unsigned int fifo_out(struct fifo *fifo, void *to, unsigned int len);
extern unsigned int fifo_out_peek(struct fifo *fifo,
				  void *to, unsigned int len, unsigned offset);

/**
 * fifo_initialized - Check if fifo is initialized.
 * @fifo: fifo to check
 * Return %true if FIFO is initialized, otherwise %false.
 * Assumes the fifo was 0 before.
 */
static inline bool fifo_initialized(struct fifo *fifo)
{
	return fifo->buffer != NULL;
}

/**
 * fifo_reset - removes the entire FIFO contents
 * @fifo: the fifo to be emptied.
 */
static inline void fifo_reset(struct fifo *fifo)
{
	fifo->in = fifo->out = 0;
}

/**
 * fifo_reset_out - skip FIFO contents
 * @fifo: the fifo to be emptied.
 */
static inline void fifo_reset_out(struct fifo *fifo)
{
	fifo->out = fifo->in;
}

/**
 * fifo_size - returns the size of the fifo in bytes
 * @fifo: the fifo to be used.
 */
static inline unsigned int fifo_size(struct fifo *fifo)
{
	return fifo->size;
}

/**
 * fifo_len - returns the number of used bytes in the FIFO
 * @fifo: the fifo to be used.
 */
static inline unsigned int fifo_len(struct fifo *fifo)
{
	register unsigned int out;

	out = fifo->out;
	return fifo->in - out;
}

/**
 * fifo_is_empty - returns true if the fifo is empty
 * @fifo: the fifo to be used.
 */
static inline int fifo_is_empty(struct fifo *fifo)
{
	return fifo->in == fifo->out;
}

/**
 * fifo_is_full - returns true if the fifo is full
 * @fifo: the fifo to be used.
 */
static inline int fifo_is_full(struct fifo *fifo)
{
	return fifo_len(fifo) == fifo_size(fifo);
}

/**
 * fifo_avail - returns the number of bytes available in the FIFO
 * @fifo: the fifo to be used.
 */
static inline unsigned int fifo_avail(struct fifo *fifo)
{
	return fifo_size(fifo) - fifo_len(fifo);
}

extern void fifo_skip(struct fifo *fifo, unsigned int len);

/*
 * __fifo_add_out internal helper function for updating the out offset
 */
static inline void __fifo_add_out(struct fifo *fifo, unsigned int off)
{
	fifo->out += off;
}

/*
 * __fifo_add_in internal helper function for updating the in offset
 */
static inline void __fifo_add_in(struct fifo *fifo, unsigned int off)
{
	fifo->in += off;
}

/*
 * __fifo_off internal helper function for calculating the index of a
 * given offeset
 */
static inline unsigned int __fifo_off(struct fifo *fifo, unsigned int off)
{
	return off & (fifo->size - 1);
}

/*
 * __fifo_peek_n internal helper function for determinate the length of
 * the next record in the fifo
 */
static inline unsigned int __fifo_peek_n(struct fifo *fifo,
					 unsigned int recsize)
{
#define __FIFO_GET(fifo, off, shift) \
	((fifo)->buffer[__fifo_off((fifo), (fifo)->out+(off))] << (shift))

	unsigned int l;

	l = __FIFO_GET(fifo, 0, 0);

	if (--recsize)
		l |= __FIFO_GET(fifo, 1, 8);

	return l;
#undef	__FIFO_GET
}

/*
 * __fifo_poke_n internal helper function for storing the length of
 * the next record into the fifo
 */
static inline void __fifo_poke_n(struct fifo *fifo,
				 unsigned int recsize, unsigned int n)
{
#define __FIFO_PUT(fifo, off, val, shift) \
		( \
		(fifo)->buffer[__fifo_off((fifo), (fifo)->in+(off))] = \
		(unsigned char)((val) >> (shift)) \
		)

	__FIFO_PUT(fifo, 0, n, 0);

	if (--recsize)
		__FIFO_PUT(fifo, 1, n, 8);
#undef	__FIFO_PUT
}

/*
 * __fifo_in_... internal functions for put date into the fifo
 * do not call it directly, use fifo_in_rec() instead
 */
extern unsigned int __fifo_in_n(struct fifo *fifo,
				const void *from, unsigned int n,
				unsigned int recsize);

extern unsigned int __fifo_in_generic(struct fifo *fifo,
				      const void *from, unsigned int n,
				      unsigned int recsize);

static inline unsigned int __fifo_in_rec(struct fifo *fifo,
					 const void *from, unsigned int n,
					 unsigned int recsize)
{
	unsigned int ret;

	ret = __fifo_in_n(fifo, from, n, recsize);

	if (ret == 0) {
		if (recsize)
			__fifo_poke_n(fifo, recsize, n);
		__fifo_add_in(fifo, n + recsize);
	}
	return ret;
}

/**
 * fifo_in_rec - puts some record data into the FIFO
 * @fifo: the fifo to be used.
 * @from: the data to be added.
 * @n: the length of the data to be added.
 * @recsize: size of record field
 *
 * This function copies @n bytes from the @from into the FIFO and returns
 * the number of bytes which cannot be copied.
 * A returned value greater than the @n value means that the record doesn't
 * fit into the buffer.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
static inline unsigned int fifo_in_rec(struct fifo *fifo,
				       void *from, unsigned int n,
				       unsigned int recsize)
{
	if (!__builtin_constant_p(recsize))
		return __fifo_in_generic(fifo, from, n, recsize);
	return __fifo_in_rec(fifo, from, n, recsize);
}

/*
 * __fifo_out_... internal functions for get date from the fifo
 * do not call it directly, use fifo_out_rec() instead
 */
extern unsigned int __fifo_out_n(struct fifo *fifo,
				 void *to, unsigned int reclen,
				 unsigned int recsize);

extern unsigned int __fifo_out_generic(struct fifo *fifo,
				       void *to, unsigned int n,
				       unsigned int recsize,
				       unsigned int *total);

static inline unsigned int __fifo_out_rec(struct fifo *fifo,
					  void *to, unsigned int n,
					  unsigned int recsize,
					  unsigned int *total)
{
	unsigned int l;

	if (!recsize) {
		l = n;
		if (total)
			*total = l;
	} else {
		l = __fifo_peek_n(fifo, recsize);
		if (total)
			*total = l;
		if (n < l)
			return l;
	}

	return __fifo_out_n(fifo, to, l, recsize);
}

/**
 * fifo_out_rec - gets some record data from the FIFO
 * @fifo: the fifo to be used.
 * @to: where the data must be copied.
 * @n: the size of the destination buffer.
 * @recsize: size of record field
 * @total: pointer where the total number of to copied bytes should stored
 *
 * This function copies at most @n bytes from the FIFO to @to and returns the
 * number of bytes which cannot be copied.
 * A returned value greater than the @n value means that the record doesn't
 * fit into the @to buffer.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
static inline unsigned int fifo_out_rec(struct fifo *fifo,
					void *to, unsigned int n,
					unsigned int recsize,
					unsigned int *total)
{
	if (!__builtin_constant_p(recsize))
		return __fifo_out_generic(fifo, to, n, recsize, total);
	return __fifo_out_rec(fifo, to, n, recsize, total);
}

/*
 * __fifo_peek_... internal functions for peek into the next fifo record
 * do not call it directly, use fifo_peek_rec() instead
 */
extern unsigned int __fifo_peek_generic(struct fifo *fifo,
					unsigned int recsize);

/**
 * fifo_peek_rec - gets the size of the next FIFO record data
 * @fifo: the fifo to be used.
 * @recsize: size of record field
 *
 * This function returns the size of the next FIFO record in number of bytes
 */
static inline unsigned int fifo_peek_rec(struct fifo *fifo,
					 unsigned int recsize)
{
	if (!__builtin_constant_p(recsize))
		return __fifo_peek_generic(fifo, recsize);
	if (!recsize)
		return fifo_len(fifo);
	return __fifo_peek_n(fifo, recsize);
}

/*
 * __fifo_skip_... internal functions for skip the next fifo record
 * do not call it directly, use fifo_skip_rec() instead
 */
extern void __fifo_skip_generic(struct fifo *fifo, unsigned int recsize);

static inline void __fifo_skip_rec(struct fifo *fifo, unsigned int recsize)
{
	unsigned int l;

	if (recsize) {
		l = __fifo_peek_n(fifo, recsize);

		if (l + recsize <= fifo_len(fifo)) {
			__fifo_add_out(fifo, l + recsize);
			return;
		}
	}
	fifo_reset_out(fifo);
}

/**
 * fifo_skip_rec - skip the next fifo out record
 * @fifo: the fifo to be used.
 * @recsize: size of record field
 *
 * This function skips the next FIFO record
 */
static inline void fifo_skip_rec(struct fifo *fifo, unsigned int recsize)
{
	if (!__builtin_constant_p(recsize))
		__fifo_skip_generic(fifo, recsize);
	else
		__fifo_skip_rec(fifo, recsize);
}

/**
 * fifo_avail_rec - returns the number of bytes available in a record FIFO
 * @fifo: the fifo to be used.
 * @recsize: size of record field
 */
static inline unsigned int fifo_avail_rec(struct fifo *fifo,
					  unsigned int recsize)
{
	unsigned int l = fifo_size(fifo) - fifo_len(fifo);

	return (l > recsize) ? l - recsize : 0;
}

/**
 * fifo_serialize - serialize the fifo struct
 * @fifo_ser: serialized fifo struct
 * @fifo: the fifo to serialize
 *
 * This function create and and alloc serialied fifo, the user need to free
 * it by itself.
 * Return size of fifo_serialized in memory, 0 on error
 */
size_t fifo_serialize(struct fifo_serialized **fifo_ser, struct fifo *fifo);

/**
 * fifo_unserialize - convert serialized fifo to a fifo struct
 * @fifo: destination fifo
 * @fifo_ser: fifo to unserialize
 *
 * The function alloc fifo buffer and need to be freed by fifo_free.
 * Return 0 on success
 */
int fifo_unserialize(struct fifo *fifo, struct fifo_serialized *fifo_ser);

#endif
