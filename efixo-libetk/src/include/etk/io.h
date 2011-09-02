/*!
 * \file etk/io.h
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

#ifndef _ETK_IO_H_
#define _ETK_IO_H_ 1

/*! \fn safe_read(int sfd, void *buf, size_t len)
 *  \brief Read from socket
 *  \param sfd Socket file descriptor
 *  \param buf Buffer to read into
 *  \param len Buffer len
 */
ssize_t safe_read(int fd, void *buf, size_t len);

/*! \fn safe_write(int sfd, void const *buf, size_t len)
 *  \brief Read from socket
 *  \param sfd Socket file descriptor
 *  \param buf Buffer to read into
 *  \param len Buffer len
 */
ssize_t safe_write(int fd, void const *buf, size_t len);

/*! \fn full_read(int sfd, void *buf, size_t len)
 *  \brief Read from socket
 *  \param sfd Socket file descriptor
 *  \param buf Buffer to read into
 *  \param len Buffer len
 */
ssize_t full_read(int fd, void *buf, size_t len);

/*! \fn full_write(int sfd, void const *buf, size_t len)
 *  \brief Read from socket
 *  \param sfd Socket file descriptor
 *  \param buf Buffer to read into
 *  \param len Buffer len
 */
ssize_t full_write(int fd, void const *buf, size_t len);

/*! \fn file_open_read_close(char const *file, void *buf, size_t len)
 *  \brief Open file, read data and close
 *  \param file Path to file
 *  \param buf Buffer to read into
 *  \param len Buffer len
 */
ssize_t file_open_read_close(char const *file, void *buf, size_t len);

/*! \fn file_open_write_close(char const *file, void const *buf, size_t len)
 *  \brief Open file, write data and close
 *  \param file Path to file
 *  \param buf Buffer to write from
 *  \param len Buffer len
 */
ssize_t file_open_write_close(char const *file, void const *buf, size_t len);

#endif
