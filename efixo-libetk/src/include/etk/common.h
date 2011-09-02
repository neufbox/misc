/*!
 * \file etk/common.h
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

#ifndef _ETK_COMMON_H_
#define _ETK_COMMON_H_ 1

#define __noret __attribute__ ((__noreturn__))

/*! \def __stringify(x)
 *  \brief return C-string value of x
 */
#define __stringify_1(x)        #x
#define __stringify(x) __stringify_1(x)

/*! \def ARRAY_SIZE
 *  \brief Count elements in static array
 */
#define ARRAY_SIZE(x) (sizeof (x) / sizeof ((x) [0]))

/*! \def ALIGN
 *  \brief Align value
 */
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))

/*! \def min(x, y)
 *  \brief Min macro that also do
 *  strict type-checking.. See the
 *  "unnecessary" pointer comparison.
 */
#define min(x, y) \
({ \
    typeof(x) _x = (x);    \
    typeof(y) _y = (y);    \
   (void) (&_x == &_y);        \
    _x < _y ? _x : _y; \
})

/*! \def max(x, y)
 *  \brief Max macro that also do
 *  strict type-checking.. See the
 *  "unnecessary" pointer comparison.
 */
#define max(x, y) \
({ \
    typeof(x) _x = (x);    \
    typeof(y) _y = (y);    \
   (void) (&_x == &_y);        \
    _x > _y ? _x : _y; \
})

/*! \def clamp(val, x, y)
 *  \brief Clamp macro that also do
 *  strict type-checking.. See the
 *  "unnecessary" pointer comparison.
 */
#define clamp(val, min, max) \
({ \
	typeof(val) __val = (val); \
	typeof(min) __min = (min); \
	typeof(max) __max = (max); \
	(void) (&__val == &__min); \
	(void) (&__val == &__max); \
	__val = __val < __min ? __min: __val; \
	__val > __max ? __max: __val; \
})

/*! \def typecheck(type, x)
 *  \brief Max macro that also do
 *  strict type-checking.. See the
 *  "unnecessary" pointer comparison.
 */
#define typecheck(type , x) \
({	type __dummy; \
	typeof(x) __dummy2; \
	(void)(&__dummy == &__dummy2); \
	1; \
})

/*! \def cast_no_const(x)
 *  \brief Remove const qualifier
 */
#define type_cast_no_const(type, x) \
	((type)(uintptr_t)(x))

/*
 * little/big endian
 */
#ifdef __BYTE_ORDER
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define le16(x) (x)
#define le32(x) (x)
#define le64(x) (x)
#define be16(x) bswap_16(x)
#define be32(x) bswap_32(x)
#elif (__BYTE_ORDER == __BIG_ENDIAN)
#define le16(x) bswap_16(x)
#define le32(x) bswap_32(x)
#define le64(x) bswap_64(x)
#define be16(x) (x)
#define be32(x) (x)
#endif
#endif /* __BYTE_ORDER */

/*
 * packed structure
 */
#ifndef PACKED
#define PACKED				__attribute__((packed))
#endif

#define time_after(a,b)         \
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(b) - (long)(a) < 0))
#define time_before(a,b)        time_after(b,a)

#define time_after_eq(a,b)      \
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(a) - (long)(b) >= 0))
#define time_before_eq(a,b)     time_after_eq(b,a)

#define time_after_time_t(a,b)         \
	(typecheck(time_t, a) &&	\
	 typecheck(time_t, b) && \
	 ((long)(b) - (long)(a) < 0))
#define time_before_time_t(a,b)        time_after_time_t(b,a)

#define time_after_eq_time_t(a,b)      \
	(typecheck(time_t, a) && \
	 typecheck(time_t, b) && \
	 ((long)(a) - (long)(b) >= 0))
#define time_before_eq_time_t(a,b)     time_after_eq_time_t(b,a)

/*
 *  * Calculate whether a is in the range of [b, c].
 *   */
#define time_in_range(a,b,c) \
	(time_after_eq(a,b) && \
	 time_before_eq(a,c))


#endif
