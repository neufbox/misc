AC_PREREQ(2.59)
AC_INIT(etk, 0.0.0, miguel.gaio@efixo.com)
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AC_CONFIG_SRCDIR([src/fork_and_exec.c])
AC_CONFIG_SRCDIR([src/safe_strtol.c])
AC_CONFIG_SRCDIR([src/safe_strtoul.c])
AC_CONFIG_SRCDIR([src/string.c])
AC_CONFIG_SRCDIR([src/strlcpy.c])
AC_CONFIG_SRCDIR([src/strlcat.c])
AC_CONFIG_SRCDIR([src/xmalloc.c])
AC_CONFIG_SRCDIR([src/read.c])
AC_CONFIG_SRCDIR([src/write.c])
AC_CONFIG_SRCDIR([src/log.c])
AC_CONFIG_SRCDIR([src/socket.c])
AC_CONFIG_SRCDIR([src/inet.c])
AC_CONFIG_SRCDIR([src/unix.c])
AC_CONFIG_SRCDIR([src/seq_file.c])
AC_CONFIG_SRCDIR([src/arp.c])
AC_CONFIG_SRCDIR([src/bridge.c])
AC_CONFIG_SRCDIR([src/net_sprint.c])
AC_CONFIG_SRCDIR([src/linux_pselect.c])
AC_CONFIG_SRCDIR([src/wrapper.c])
AC_CONFIG_SRCDIR([src/mme.c])
AC_CONFIG_SRCDIR([src/xdev.c])
AC_CONFIG_SRCDIR([src/http.c])
AC_CONFIG_SRCDIR([src/dict.c])
AC_CONFIG_SRCDIR([src/pack.c])
AC_CONFIG_HEADER([config.h])

LT_INIT
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

# Checks for libraries
AC_CHECK_LIB(tropicssl, md5_starts,,echo no md5_starts in tropicssl?\; exit)
AC_CONFIG_FILES([Makefile
		 src/Makefile])

AC_OUTPUT
