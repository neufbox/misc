# Makefile

# installation variables
prefix	= /usr
includedir = $(prefix)/include
libdir	= $(prefix)/lib
bindir	= $(prefix)/bin

# target config
TGTNAME	:= libewf
TGTVER	:= 0.0.0

# global variables
INSTALL	:= /usr/bin/install

# compilation variables
LIBSDYN := -lm -lz -lpcre -lezxml -lpthread -ltropicssl
LIBSSTA := -lfcgi -lneo_cgi -lneo_cs -lneo_utl

CFLAGS	:= -I./include/ $(CFLAGS) -fPIC -I$(includedir)/ClearSilver/ \
	   -D_GNU_SOURCE -DLIBEWF_VERSION=\"$(TGTVER)\"

ifdef DEBUG
CFLAGS	+= -O0 -g -DDEBUG -DDMALLOC -DDMALLOC_FUNC_CHECK \
	   -Wall -Wextra -Werror -Wbad-function-cast -Wshadow \
	   -Wcast-qual -Wold-style-definition -Wmissing-noreturn \
	   -Wstrict-prototypes -Waggregate-return -Wformat=2 \
	   -Wundef -Wbad-function-cast -Wunused-parameter -Wnonnull
LDFLAGS	+= -Wall -fPIC
LIBSSTA += -ldmalloc -lnbu
else
CFLAGS	+= -Os -DNDEBUG
LDFLAGS	+= -Wall
LIBSDYN += -lnbu
endif

LDLIBS	:= -Wl,-Bstatic $(LIBSSTA) -Wl,-Bdynamic $(LIBSDYN)

export prefix
export bindir
export INSTALL
export CFLAGS
export LDFLAGS
export LIBSSTA
export LIBSDYN

############################################################
# targets
tgt_a	= $(TGTNAME).a
tgt_so	= $(TGTNAME).so
tgt_so_wver = $(TGTNAME).so.$(TGTVER)
tgts	= $(tgt_so_wver) $(tgt_so) $(tgt_a)

ifdef DEBUG
tgts	+= tests
endif

# list C files
sources	= $(wildcard src/*.c) $(wildcard src/core/*.c)

# get objects from C files
objects	= $(sources:.c=.o)

############################################################
# rules
.PHONY: all
all: build

.PHONY: build
build: $(tgts)

.PHONY: tests
tests: $(tgt_a)
	make -C tests/ install clean

$(tgt_so_wver): $(objects)
	$(CC) $(LDFLAGS) -shared $^ $(LDLIBS) -o $@

$(tgt_so): $(tgt_so_wver)
	ln -s $(tgt_so_wver) $(tgt_so)

$(tgt_a): $(objects)
	$(AR) rcs $@ $^

.PHONY: clean
clean:
	find -name "*.o" -delete
	rm -f $(tgts)

.PHONY: mrproper
mrproper: clean

.PHONY: install
install: build
	$(INSTALL) -d $(libdir)/
	$(INSTALL) -m 755 $(tgt_so_wver) $(tgt_so) $(tgt_a) $(libdir)/
	$(INSTALL) -d $(includedir)/
	cp -R include/* $(includedir)/

.PHONY: uninstall
uninstall:
	$(RM) -f $(libdir)/$(tgt_so)
