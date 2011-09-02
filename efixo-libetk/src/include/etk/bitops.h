/*!
 * \file etk/bitops.h
 *
 * \author Copyright 2010 Miguel GAIO <miguel.gaio@efixo.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _ETK_BITOPS_H_
#define _ETK_BITOPS_H_ 1

/*! \def BIT_MASK(nr)
 *  \brief bit mask
 */
#define BIT_MASK(nr) (1 << (nr))

/*! \fn change_bit - Set a bit in memory
 */
static inline void set_bit(int nr, volatile unsigned long *addr)
{
	*addr |= BIT_MASK(nr);
}

/*! \fn change_bit - Clear a bit in memory
 */
static inline void clear_bit(int nr, volatile unsigned long *addr)
{
	*addr &= ~BIT_MASK(nr);
}

/*! \fn change_bit - Toggle a bit in memory
 */
static inline void change_bit(int nr, volatile unsigned long *addr)
{
	*addr ^= BIT_MASK(nr);
}

/*! \fn test_and_set_bit - Set a bit and return its old value
 */
static inline int test_and_set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long old = *addr;

	*addr = old | mask;
	return (old & mask) != 0;
}

/*! \fn Clear a bit and return its old value
 */
static inline int test_and_clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long old = *addr;

	*addr = old & ~mask;
	return (old & mask) != 0;
}

/*! \fn Toggle a bit and return its old value
 */
static inline int test_and_change_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long old = *addr;

	*addr = old ^ mask;
	return (old & mask) != 0;
}

/*! \fn Determine whether a bit is set
 */
static inline int test_bit(int nr, const volatile unsigned long *addr)
{
	return 1UL & (*addr >> nr);
}

#endif
