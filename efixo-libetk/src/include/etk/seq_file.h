/*!
 * \file etk/seq_file.h
 *
 * \brief  Declare seq_file interface(struct seq_file)
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
 * 
 * $Id: seq_file.h 16828 2010-07-13 16:08:44Z mgo $
 */

#ifndef _ETK_SEQ_FILE_H_
#define _ETK_SEQ_FILE_H_

#include <stdint.h>

/*! \struct seq_file
 *  \brief seq_file descriptor (seq_file.c)
 */
struct seq_file {
	/*! \brief buf buffer */
	char *buf;

	/*! \brief size buffer size */
	size_t size;

	/*! \brief count current pos */
	size_t count;
};

/*! \def DEFINE_SEQFILE(name, P, S)
 *  \brief Define seq_file struct
 */
#define DEFINE_SEQFILE(x,P,S) 	\
struct seq_file x = {	\
	.buf = P,	\
	.size = S	\
}

/*! \fn seq_printf(struct seq_file *m, const char *f, ...)
 *  \brief Print data into seq_file.
 *  \param m seq_file reference
 *  \param f format
 *  \param ... args
 */
int seq_printf(struct seq_file *m, const char *f, ...)
    __attribute__ ((__format__(__printf__, 2, 3)));

/*! \fn seq_print_hex(struct seq_file *m, uint8_t const *bin, size_t len)
 *  \brief Print data into seq_file.
 *  \param m seq_file reference
 *  \param bin data pointer
 *  \param len data len
 */
void seq_print_hex(struct seq_file *m, uint8_t const *bin, size_t len);

/*! \enum mac_addr_format
 *  \brief Enum the different format existing for mac address
 */
enum mac_addr_format {
	/*! \brief 00:17:33:80:00:34 */
	mac_addr_format_system,
	/*! \brief 00-17-33-80-00-34 */
	mac_addr_format_voip,
	/*! \brief 001733800034 */
	mac_addr_format_compact,
};

/*! \fn seq_print_mac_addr(struct seq_file *m, uint8_t *mac, enum mac_addr_format fmt, int off)
 *  \brief Print a mac address into seq_file.
 *  \param m seq_file reference
 *  \param mac mac address (array of 6 bytes)
 *  \param fmt format wanted for the mac address
 *  \param off mac offset
 */
void seq_print_mac_addr(struct seq_file *m, uint8_t const *mac,
			enum mac_addr_format fmt, int off);

/*! \fn seq_print_net(struct seq_file *m, char const *s_ipaddr, char const *s_netmask)
 *  \brief Print Network
 *  \param m seq_file reference
 *  \param s_ipaddr network ip address
 *  \param s_network network netmask address
 */
int seq_print_net(struct seq_file *m, char const *s_ipaddr,
		  char const *s_netmask);

/*! \fn seq_c_str(struct seq_file const *m)
 *  \brief Give C-string
 *  \ param m seq_file reference
 */
static inline char const *seq_c_str(struct seq_file const *m)
{
	return m->buf;
}

/*! \fn seq_clear(struct seq_file *m)
 *  \brief Clear the seq_file.
 *  \param m seq_file reference
 */
static inline void seq_clear(struct seq_file *m)
{
	m->count = 0;
}

/*! \fn seq_empty(struct seq_file *m)
 *  \brief Return true if a seq_file is empty.
 *  \param m seq_file reference
 */
static inline int seq_length(struct seq_file const *m)
{
	return m->count;
}

/*! \fn seq_empty(struct seq_file *m)
 *  \brief Return true if a seq_file is empty.
 *  \param m seq_file reference
 */
static inline int seq_empty(struct seq_file const *m)
{
	return (m->count == 0UL);
}

/*! \fn seq_full(struct seq_file *m)
 *  \brief Return true if a seq_file is full.
 *  \param m seq_file reference
 */
static inline int seq_full(struct seq_file const *m)
{
	return (m->count == m->size);
}

#endif
