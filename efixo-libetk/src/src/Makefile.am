lib_LTLIBRARIES = libetk.la
libetk_la_SOURCES = fork_and_exec.c \
		    safe_strtol.c \
		    safe_strtoul.c \
		    string.c \
		    strlcpy.c \
		    strlcat.c \
		    xmalloc.c \
		    log.c \
		    socket.c \
		    inet.c \
		    inet_dns.c \
		    unix.c \
		    read.c \
		    write.c \
		    seq_file.c \
		    arp.c \
		    bridge.c \
		    net_sprint.c \
		    linux_pselect.c \
		    wrapper.c \
		    mme.c \
		    xdev.c \
		    dict.c \
		    fifo.c \
		    http.c \
		    pack.c

AM_CFLAGS  = -I. -std=gnu99 \
	   -Wall -Wextra -Werror -Wbad-function-cast -Wshadow \
	   -Wcast-qual -Wold-style-definition -Wmissing-noreturn \
	   -Wstrict-prototypes -Waggregate-return -Wformat=2 \
	   -Wundef -Wbad-function-cast -Wunused-parameter -Wnonnull
INCLUDES = -I$(top_srcdir)/include

libetk_la_LIBADD = -ltropicssl
libetk_la_LFFLAGS =

indent:
	find -name "*.[c|h]" | xargs indent -linux
	find -name "*~" -delete
