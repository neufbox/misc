/*
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

static void usage(void)
{
	fprintf(stderr, "iomem <address> <size> [<value>]\n");
	exit(EXIT_FAILURE);
}

static void hexdump(char const *base, char const *p, int len)
{
	for (; len; len -= 16) {
		int end;

		if (len < 0)
			break;

		end = min(16, len);

		printf("%08lx: ", (unsigned long)base);
		base += 16;

		for (int i = 0; i < end; ++i) {
			if (!(i % 4))
				printf(" ");
			printf(" %02x", (unsigned)(unsigned char)*p++);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	const size_t page_size = sysconf(_SC_PAGESIZE);
	int width = 4, set = 0, fd;
	unsigned long addr, value;
	void *page;
	void *uaddr;

	if (argc < 3)
		usage();

	addr = strtoul(argv[1], 0, 16);
	if (addr < 0xfff00000) {
		addr &= ~0xe0000000;
		if ((addr < 0x10000000) || (addr > 0x1f0000000)) {
			fprintf(stderr, "out of range address %lx\n", addr);
			exit(EXIT_FAILURE);
		}
	}

	width = strtoul(argv[2], 0, 0);
	if (argc > 3) {
		set = 1;
		value = strtoul(argv[3], 0, 16);
	}

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "cannot open %s\n", "/dev/mem");
		exit(EXIT_FAILURE);
	}

	width = min(width, page_size);
	page = mmap(0, 2 * page_size, PROT_READ | PROT_WRITE,
		    MAP_SHARED, fd, addr & (~(page_size - 1)));
	if (page == MAP_FAILED) {
		fprintf(stderr, "cannot mmap region\n");
		exit(EXIT_FAILURE);
	}

	uaddr = (void *)(((unsigned long)page) + (addr & (page_size - 1)));
	switch (width) {
	case 4:
		if (set)
			*(unsigned *)uaddr = value;
		printf("%08lx: %08x\n", addr, (unsigned)(*(unsigned *)uaddr));
		break;
	case 2:
		if (set)
			*(unsigned short *)uaddr = value;
		printf("%08lx: %04x\n", addr,
		       (unsigned)(*(unsigned short *)uaddr));
		break;
	case 1:
		if (set)
			*(unsigned char *)uaddr = value;
		printf("%08lx: %02x\n", addr,
		       (unsigned)(*(unsigned char *)uaddr));
		break;
	default:
		hexdump(addr, uaddr, width);
		break;
	}

	munmap(page, 2 * page_size);

	return 0;
}
