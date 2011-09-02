ifndef TOPDIR
TOPDIR=.
endif

include $(TOPDIR)/.config

################################
# GENERAL API CONFIG
#
TGT = fcgiroot/apid

################################
# FILES
#
CMN = api_main api_common api_signal ewf_hashtable ewf_fastcgi ewf_http \
      ewf_crypto ewf_session nbl_trash cfg

CTLS = auth

# ctl files
define Ctl/Select
  ifeq ($(CONFIG_efixo-www-api-ctl-$(1)), y)
    CTLS += $(1)
  endif
endef

CTLS_AVAIL := $(basename $(notdir $(wildcard src/ctl/*.c)))
$(foreach ctl,$(CTLS_AVAIL),$(eval $(call Ctl/Select,$(ctl))))

# defs
SRC = $(addprefix src/, $(addsuffix .c, $(addprefix ctl/, $(CTLS))) $(addsuffix .c, $(CMN)))

INC = -Isrc -I$(stagingdir)/include -I$(stagingdir)/usr/include \
	-I$(stagingdir)/include/fastcgi -I$(stagingdir)/include/clearsilver \
	-I$(stagingdir)/usr/include/ClearSilver

installdir	:= $(ROOT)
sbindir		:= $(installdir)/sbin
bindir		:= $(installdir)/bin
etcdir		:= $(installdir)/etc
libdir		:= $(installdir)/lib
stagingdir	:= $(STAGING_DIR)
includedir	:= $(stagingdir)/include
bindir		:= $(installdir)

ifdef DEBUG
CFLAGS          += -g
CFLAGS          += -Wall -Wextra -Wbad-function-cast -Wshadow \
                   -Wcast-qual -Wold-style-definition -Wmissing-noreturn \
                   -Wstrict-prototypes -Waggregate-return -Wformat=2 \
                   -Wswitch-default -Wundef -Wbad-function-cast \
                   -Wunused-parameter -Wnonnull
CFLAGS          += -DDEBUG
# -DDMALLOC -DDMALLOC_FUNC_CHECK
LDFLAGS         += -Wall

# must be in this order: -lfcgi -lneo_cgi -lneo_cs -lneo_utl -lm
LIBSTATIC	+= -lfcgi -lneo_cgi -lneo_cs -lneo_utl -lm -lnbd -lpthread -lnbu -ldmalloc -letk
LIBDYNAMIC	+= -lezxml -lz -ltropicssl
else
CFLAGS 		+= -std=gnu99 -s -Os -DNDEBUG -D_GNU_SOURCE -DNBD_USE_DEPRECATED_API

LIBSTATIC       += -lfcgi -lneo_cgi -lneo_cs -lneo_utl -letk
LIBDYNAMIC      += -lezxml -lnbd -lnbu -lz -lm -ltropicssl
endif

BOX_CFLAGS  	= -DBOXNAME=\"${BOX}\" \
		  -D$(shell echo "${BOX}" | tr "[:lower:]" "[:upper:]")

CFLAGS          += $(INC) $(BOX_CFLAGS) $(CONF_CFLAGS) -std=gnu99 \
	 	   -D_GNU_SOURCE -D__USER__="\"${USER}\""
LDFLAGS         += -L$(stagingdir)/lib -L$(stagingdir)/usr/lib
LDLIBS		:= -Wl,-Bstatic $(LIBSTATIC) -Wl,-Bdynamic $(LIBDYNAMIC)

INSTALL         = /usr/bin/install

################################################################

.PHONY: install clean tests

all: $(TGT)

sources := $(SRC)

objects := $(sources:.c=.o)

$(TGT): $(objects)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

tests:
	make -C src/tests

install:
	@echo Creating www directory...
	@$(INSTALL) -d $(installdir)/www
	@$(INSTALL) -d $(installdir)/www/fcgiroot

	@echo Installing executable...
	@$(INSTALL) -m 755 $(TGT) $(installdir)/www/$(TGT)

	@echo Installing cst templates...
	@cp -rf fcgiroot/api $(installdir)/www/fcgiroot

indent:
	@echo "indent -kr -i4 -c4 -prs -nce -l100 ..."
	@indent -kr -i4 -c4 -prs -nce -l100 $(SRC) > /dev/null
	@find -name "*~" -delete

clean:
	rm -rf $(TGT) doc/html *~
	rm -rf $(installdir)/www/$(TGT)
	rm -rf $(installdir)/www/fcgiroot/api
	-rmdir --ignore-fail-on-non-empty $(installdir)/www/fcgiroot $(installdir)/www
