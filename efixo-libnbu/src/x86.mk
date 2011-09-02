export CC		:= gcc
export AR		:= ar
export STRIP	:= strip

MACH		:= bcm963xx
SOCKETFILE	:= $(INSTALL_DIR)/var/run/nbd.socket
DEV_EVENTS	:= $(INSTALL_DIR)/dev/events
DEV_LEDS	:= $(INSTALL_DIR)/dev/leds

# Specific plugins
PLUGINS		+= # TODO 
NVRAM_SYSTEM_VERSION:=00.00
NVRAM_USER_VERSION:=00.00

prefix		:= $(TARGETS_DIR)/$(PROFILE)/install
exec_prefix	:= $(prefix)
sbindir		:= $(exec_prefix)/sbin
bindir		:= $(exec_prefix)/bin
libdir		:= $(exec_prefix)/lib
kerneldir	:= $(KERNEL_DIR)
stagingdir	:= $(STAGING_DIR)
includedir	:= $(stagingdir)/include

EXTRA_CFLAGS	:= -I/usr/include -DX86
