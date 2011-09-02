/*!
 * \file etk/vt102.h
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

#ifndef _ETK_VT102_H_
#define _ETK_VT102_H_ 1


#define __BLACK(txt)		"\033[0;30m" txt "\033[0m"
#define __RED(txt)		"\033[0;31m" txt "\033[0m"
#define __GREEN(txt)		"\033[0;32m" txt "\033[0m"
#define __BROWN(txt)		"\033[0;33m" txt "\033[0m"
#define __BLUE(txt)		"\033[0;34m" txt "\033[0m"
#define __PURPLE(txt)		"\033[0;35m" txt "\033[0m"
#define __CYAN(txt)		"\033[0;36m" txt "\033[0m"
#define __GREY(txt)		"\033[0;37m" txt "\033[0m"

#define __DARK_GREY(txt)	"\033[1;30m" txt "\033[0m"
#define __LIGHT_RED(txt)	"\033[1;31m" txt "\033[0m"
#define __LIGHT_GREEN(txt)	"\033[1;32m" txt "\033[0m"
#define __YELLOW(txt)		"\033[1;33m" txt "\033[0m"
#define __LIGHT_BLUE(txt)	"\033[1;34m" txt "\033[0m"
#define __LIGHT_PURPLE(txt)	"\033[1;35m" txt "\033[0m"
#define __LIGHT_CYAN(txt)	"\033[1;36m" txt "\033[0m"
#define __WHITE(txt)		"\033[1;37m" txt "\033[0m"

#endif
