
LDLIBS += -lrt -lezxml -letk -lnl-tiny -lnbd

CFLAGS+= -std=gnu99 -D_GNU_SOURCE -Wall -Werror -Wshadow \
	  -I$(STAGING_DIR)/usr/include/libnl-tiny/ \
	  -I$(LINUX_DIR)/arch/mips/include/asm/mach-bcm63xx/

lan-topology: lan-topology.o neigh.o plc.o ssdp.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

INDENT_ARGS	:= -linux
.PHONY: indent
indent:
	find -name "*.[c|h]" | xargs indent $(INDENT_ARGS) > /dev/null
	find -name "*~" -delete
