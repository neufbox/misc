/*
 * Math related operations
 *
 * Copyright (C) 2010 Tanguy Bouzéloc <tanguy.bouzeloc@efixo.com>
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
#ifndef _MATH_H_
#define _MATH_H_
#include <stdbool.h>

/**
 * roundup_pow_of_two - round the given value up to nearest power of two
 * @n - parameter
 *
 * round the given value up to the nearest power of two
 * - the result is undefined when n == 0
 * - this can be used to initialise global variables from constant data
 */
static inline __attribute__ ((const))
unsigned long roundup_pow_of_two(unsigned long n)
{
	return 1UL << (__builtin_clz(n - 1) + 1);
}

/*
 *  Determine whether some value is a power of two, where zero is
 * *not* considered a power of two.
 */
static inline __attribute__ ((const))
bool is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

#endif				/* _MATH_H_ */
