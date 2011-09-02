#
# COMMIT 10000 REPRESENT(c)
#
ifndef TOPDIR
TOPDIR=.
endif

include $(TOPDIR)/.config

################################
# GENERAL WEBUI CONFIG
#
TGT 		= fcgiroot/fastcgi
VERSION 	= 2.4
LANGS 		= fr en

THM 		= default

OBJDIR		= .obj
BUILDROOTDIR	= .buildroot
SRCDIR		= src

################################
# FILES
#
# cmn files
CMN             = main nbl_tv nbl_wlan nbl_plug nbl_net nbl_voip nbl_utils \
                  nbl_exim dbl

WUI_CMN		= index error upgrading reboot login logout rootfs lanpopup

JS_CMN   	= global.js
JS_CONCAT_CMN	= jquery.js jquery.modal.js jssha256.js common.js

CSS_CMN  	= csshover.htc reset.css state.ie6.css

CST		= defs.cst
HTML_CMN	= footer.html header.html menu.html
HDF_LNG_CMN 	= menu.hdf text.hdf

THM_CMN		= favicon.ico theme.xml preview.png css/common.css

# helper files
define ExtraCmn/Select
  ifeq ($(CONFIG_efixo-www-extracmn-$(1)), y)
    EXTRACMN_ENABLED += $(1)
  endif
endef

EXTRACMN_AVAIL := $(filter-out $(CMN), $(basename $(notdir $(wildcard $(SRCDIR)/*.c))))
$(foreach extracmn,$(EXTRACMN_AVAIL),$(eval $(call ExtraCmn/Select,$(extracmn))))

CMN += $(EXTRACMN_ENABLED)

# ctl files
define Page/Select
  ifeq ($(CONFIG_efixo-www-page-$(1)), y)
    PAGES_FLAGS += -Dpage_$(1)
    PAGES_ENABLED += $(1)
    PAGES_ENABLED_URI += $(subst _,/,$(1))
  endif
endef

PAGES_AVAIL := $(basename $(notdir $(wildcard $(SRCDIR)/wui/*.c)))
$(foreach page,$(PAGES_AVAIL),$(eval $(call Page/Select,$(page))))

WUI := $(WUI_CMN) $(PAGES_ENABLED)
WUI_RURI := $(subst _,/,$(WUI_CMN)) $(PAGES_ENABLED_URI)

ifeq ($(strip $(CONFIG_efixo-www-kit)),y)
KIT = kit_helper get set action
KIT_RURI := $(addprefix kit/, $(KIT))
endif

ifeq ($(strip $(CONFIG_efixo-www-stb)),y)
STB = stb_info
STB_RURI := $(subst _,/,$(STB))
endif

ifeq ($(strip $(CONFIG_efixo-www-wdg)),y)
WDG = lan leds system wlan
WDG_RURI := $(subst _,/,$(WDG))
endif

################################
# some private defs
VPATH		= $(OBJDIR)

FORBIDDEN_FUNCS = basename sprintf snprintf strcpy strncpy

installdir	:= $(ROOT)
sbindir		:= $(installdir)/sbin
bindir		:= $(installdir)/bin
etcdir		:= $(installdir)/etc
libdir		:= $(installdir)/lib
stagingdir	:= $(STAGING_DIR)
includedir	:= $(stagingdir)/include
bindir		:= $(installdir)

CONF_CFLAGS 	= -DWEBUI_VERSION=\"${VERSION}\" \
	          -DFASTCGI_PIDFILE=\"/var/run/fastcgi.pid\"

ifeq ($(strip $(CONFIG_WEBUI_USB_SUPPORT)),y)
CONF_CFLAGS     += -DHAVE_USB_SUPPORT
endif

ifeq ($(strip $(CONFIG_WEBUI_LED_SUPPORT)),y)
CONF_CFLAGS     += -DHAVE_LED_SUPPORT
endif

ifeq ($(strip $(CONFIG_WEBUI_IPV6_SUPPORT)),y)
CONF_CFLAGS     += -DHAVE_IPV6_SUPPORT
endif

ifeq ($(strip $(CONFIG_WEBUI_BACKUP_3G_SUPPORT)),y)
CONF_CFLAGS     += -DHAVE_BACKUP_3G_SUPPORT
endif

ifeq ($(strip $(CONFIG_WEBUI_HOTSPOT_FON_SUPPORT)),y)
CONF_CFLAGS     += -DHAVE_HOTSPOT_FON_SUPPORT
endif

BOX_CFLAGS  	= -DUBOX=\"${BOX}\" -D$(shell echo "${BOX}" | tr "[:lower:]" "[:upper:]")

ifdef DEBUG
LIBSTATIC	:= -lewf -ldmalloc -lfcgi -lneo_cgi -lneo_cs -lneo_utl \
	 -lnbu -lnbd -letk
LIBDYNAMIC	:= -lm -lz -lpcre -lpthread -lezxml -ltropicssl
else
LIBSTATIC	:= -lfcgi -lneo_cgi -lneo_cs -lneo_utl
LIBDYNAMIC	:= -lm -lz -lpcre -lpthread -lezxml -lnbu -lnbd \
	-lewf -letk -ltropicssl
endif

################################
# global cc opts
INC 		= -Isrc -I$(stagingdir)/include \
                  -I$(stagingdir)/usr/include \
		  -I$(stagingdir)/include/fastcgi \
                  -I$(stagingdir)/include/ClearSilver \
		  -I$(stagingdir)/usr/include/ClearSilver

CFLAGS 		+= $(INC) $(BOX_CFLAGS) $(CONF_CFLAGS) -std=gnu99 \
                   -D_GNU_SOURCE -D__USER__="\"${USER}\"" \
                   -DNBD_USE_DEPRECATED_API
LDFLAGS 	+= ${TOOLCHAIN_ABI} -L$(stagingdir)/lib -L$(stagingdir)/usr/lib

ifneq ($(LIBSTATIC),)
LDLIBS		+= -Wl,-Bstatic $(LIBSTATIC)
endif

ifneq ($(LIBDYNAMIC),)
LDLIBS 		+= -Wl,-Bdynamic $(LIBDYNAMIC)
endif

INSTALL		= /usr/bin/install

CFLAGS		+= -Wall -Wextra -Werror -Wbad-function-cast -Wshadow \
                   -Wcast-qual -Wold-style-definition -Wmissing-noreturn \
                   -Wstrict-prototypes -Waggregate-return -Wformat=2 \
                   -Wswitch-default -Wundef -Wbad-function-cast \
                   -Wunused-parameter -Wnonnull

ifdef DEBUG
MAKEFLAGS	+= 'DEBUG=y'
CFLAGS		+= -g
DEBUG_FLAG	+= -DDEBUG
CFLAGS		+= $(DEBUG_FLAG) -DDMALLOC -DDMALLOC_FUNC_CHECK
LDFLAGS		+= -Wall
WUI		+= segfault test
WUI_RURI	+= segfault test
SUBTGTS		+= tests
JSSTRIP		= cat
else
MAKEFLAGS	+=
CFLAGS		+= -Os -DNDEBUG
LDFLAGS		+= -s
JSSTRIP		= jsmin
endif

################################
# the sources file
# c -> o
PGS		= $(addprefix wui/, $(WUI)) $(addprefix kit/, $(KIT)) $(addprefix stb/, $(STB)) $(addprefix wdg/, $(WDG))
PGS_SRC		= $(addprefix $(SRCDIR)/, $(addsuffix .c, $(PGS)))
PGS_OBJ		= $(PGS_SRC:.c=.o)
CMN_SRC		= $(addprefix $(SRCDIR)/, $(addsuffix .c, $(CMN)))
CMN_OBJ		= $(CMN_SRC:.c=.o)
CMN_WITHOUT_MAIN_OBJ = $(filter-out src/main.o, $(CMN_OBJ))
SRC 		= $(PGS_SRC) $(CMN_SRC)
OBJ 		= $(PGS_OBJ) $(CMN_OBJ)

# hdf lng/rgx
FCGI_WUI_LNG	= $(foreach dir, $(addprefix fcgiroot/lng/, $(LANGS)), $(addprefix $(dir)/, $(addsuffix .hdf, $(WUI_RURI)) $(HDF_LNG_CMN)))
FCGI_WUI_LNG_AVL= $(foreach dir, $(addprefix fcgiroot/lng/, $(LANGS)), $(shell find $(dir) -name "*.hdf"))

FCGI_WUI_RGX	= $(addprefix fcgiroot/rgx/, $(addsuffix .hdf, $(WUI_RURI)))
FCGI_WUI_RGX_AVL= $(shell find fcgiroot/rgx/ -name "*.hdf")

# cst/html/xml
FCGI_WUI_TPL	= $(addprefix fcgiroot/tpl/, $(CST) $(HTML_CMN) $(addsuffix .html, $(KIT_RURI) $(STB_RURI) $(WDG_RURI) $(WUI_RURI)) $(addsuffix .xml, $(KIT_RURI) $(STB_RURI) $(WDG_RURI) $(WUI_RURI)))
FCGI_WUI_TPL_AVL= $(shell find fcgiroot/tpl/ \( -name "*.html" -o -name "*.cst" -o -name "*.xml" \))

# fcgiroot files
FCGI_WUI	= $(filter $(FCGI_WUI_LNG), $(FCGI_WUI_LNG_AVL)) $(filter $(FCGI_WUI_RGX), $(FCGI_WUI_RGX_AVL)) $(filter $(FCGI_WUI_TPL), $(FCGI_WUI_TPL_AVL))

# css
DOC_WUI_CSS	= $(addprefix docroot/css/, $(addsuffix .css, $(WUI_RURI)) $(CSS_CMN))
DOC_WUI_CSS_AVL = $(shell find docroot/css/ -name "*.css")

# js
DOC_WUI_JS	= $(addprefix docroot/js/, $(addsuffix .js, $(WUI_RURI)) $(JS_CMN))
DOC_WUI_JS_CONCAT = $(addprefix docroot/js/, $(JS_CONCAT_CMN))
DOC_WUI_JS_AVL	= $(shell find docroot/js/ -name "*.js") $(addprefix docroot/js/, $(JS_CMN))

# docroot files
DOC_WUI		= $(filter $(DOC_WUI_CSS), $(DOC_WUI_CSS_AVL)) $(filter $(DOC_WUI_JS), $(DOC_WUI_JS_AVL))

DOC_THM_IMG	= $(shell ls docroot/themes/$(THM)/img/)
DOC_THM		= $(addprefix docroot/themes/, $(addprefix $(THM)/, $(THM_CMN) $(addprefix img/, $(DOC_THM_IMG))))

################################
# preprocess configuration
PREPROC_ARGS 	= -U "" "" "\(" "," "\)" "\(" "\)" "\#" "" +c "/*" "*/" -M "\#" "\n" " " " " "\n" "" ""
PREPROC 	= gpp $(PREPROC_ARGS) $(BOX_CFLAGS) $(CONF_CFLAGS) $(DEBUG_FLAG) $(PAGES_FLAGS)

PREPROC_FILE 	= $(FCGI_WUI) $(DOC_WUI)
PREPROC_RESULT	= $(addprefix $(BUILDROOTDIR)/, $(PREPROC_FILE))

################################
# no C file targets
TGT_NO_C_FILE	= $(PREPROC_RESULT) $(addprefix $(BUILDROOTDIR)/, $(DOC_THM))
TGT_JS_TO_CONCAT= $(addprefix $(BUILDROOTDIR)/, $(DOC_WUI_JS_CONCAT))

################################
# export
export CMN_WITHOUT_MAIN_OBJ
export OBJDIR
export CFLAGS
export LDFLAGS
export LDLIBS

#debug:
# 	@echo "EXTRACMN_ENABLED = $(EXTRACMN_ENABLED)"
#	@echo "PAGES_ENABLED = $(PAGES_ENABLED)"
#	@echo "PAGES_ENABLED_URI = $(PAGES_ENABLED_URI)"
#	@echo "WUI = $(WUI)"
#	@echo "WUI_RURI = $(WUI_RURI)"
#	@echo "FCGI_WUI_TPL = $(FCGI_WUI_TPL)"
#	@echo "FCGI_WUI_TPL_AVL = $(FCGI_WUI_TPL_AVL)"
#	@echo "FCGI_WUI = $(FCGI_WUI)"
#	@echo "DOC_WUI = $(DOC_WUI)"
# 	@echo "CMN = $(CMN)"
# 	@echo "CONF_CFLAGS = $(CONF_CFLAGS)"
# 	@echo "CONFIG_WEBUI_USB_SUPPORT = $(CONFIG_WEBUI_USB_SUPPORT)"
# 	@echo "CONFIG_WEBUI_THEME = $(CONFIG_WEBUI_THEME)"
#	@echo "TGT_NO_C_FILE = $(TGT_NO_C_FILE)"

################################
# begin the rules
################################

.PHONY: all
all: check buildroot $(TGT) $(SUBTGTS)
	@cp $(TGT) $(BUILDROOTDIR)/fcgiroot/
	@echo "**********************************************"
	@echo "*            COMPILATION FINISH              *"
	@echo "*                                            *"
ifdef DEBUG
	@echo "*       - Compiled with DEBUG flag           *"
endif
	@echo "**********************************************"
	@echo "* USB support: $(CONFIG_WEBUI_USB_SUPPORT)"
	@echo "* LED support: $(CONFIG_WEBUI_LED_SUPPORT)"
	@echo "* IPV6 support: $(CONFIG_WEBUI_IPV6_SUPPORT)"
	@echo "* HOTSPOT FON support: $(CONFIG_WEBUI_HOTSPOT_FON_SUPPORT)"
	@echo "* Theme: $(THM)"
	@echo "* Kit interface: $(CONFIG_efixo-www-kit)"
	@echo "* Stb interface: $(CONFIG_efixo-www-stb)"
	@echo "* Wdg interface: $(CONFIG_efixo-www-wdg)"
	@echo "**********************************************"

$(TGT): $(addprefix $(OBJDIR)/, $(OBJ))
	@echo "  output $@"
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

$(OBJDIR)/%.o: %.c
	@echo "  cc $<"
	@mkdir -p `dirname $@`
	@${CC} ${CFLAGS} -c $< -o $@

.PHONY: buildroot
buildroot: $(TGT_NO_C_FILE)
	@echo "  update buildroot ok"

# js minified this files
$(BUILDROOTDIR)/docroot/js/jquery.js: docroot/js/jquery.js
	@mkdir -p `dirname $@`
	@$(JSSTRIP) < $< > $@
	@echo >> $@

$(BUILDROOTDIR)/docroot/js/jquery.%.js: docroot/js/jquery.%.js
	@mkdir -p `dirname $@`
	@$(JSSTRIP) < $< > $@
	@echo >> $@

$(BUILDROOTDIR)/docroot/js/%: docroot/js/%
	@mkdir -p `dirname $@`
	@${PREPROC} $< | $(JSSTRIP) > $@
	@echo >> $@

# concatenate common javascript files in one file
$(BUILDROOTDIR)/docroot/js/global.js: $(TGT_JS_TO_CONCAT)
	@cat $^ > $@
	@rm -f $^

$(BUILDROOTDIR)/docroot/themes/%: docroot/themes/%
	@mkdir -p `dirname $@`
	@cp $^ $@

$(BUILDROOTDIR)/fcgiroot/tpl/defs.cst: fcgiroot/tpl/defs.cst
	@mkdir -p `dirname $@`
	@${PREPROC} $^ > $@
# remove \n and facultative spaces
	@perl -p -i -e 's/(\n|  )//g' $@

$(BUILDROOTDIR)/%: %
	@mkdir -p `dirname $@`
	@${PREPROC} $^ > $@

.PHONY: check
check:
	@for p in $(FORBIDDEN_FUNCS); \
	do \
		echo "*** Checking for use of '$$p' function."; \
		grep -n " $$p" $(SRC); \
		if [ "$$?" = 0 ]; \
		then \
			echo "*** Arrrgghhh!!! Don't use '$$p' in your code, it's unsafe."; \
			exit 1; \
		fi; \
	done

	@echo "  check ok"

.PHONY: pruning
pruning:
	find ./src/ \( -iname "*.c" -o -iname "*.h" \) -exec perl -p -i -e 's/[ \t]*$//g' {} \;

.PHONY: doc
doc:
	doxygen doc/doxygen.conf

.PHONY: tests
tests:
	make -C tests

.PHONY: install
install:
	@echo "Cleaning..."
	@rm -rf $(installdir)/www

	@echo "Creating www directory..."
	@$(INSTALL) -d $(installdir)/www

	@echo "Installing files..."
	@cp -r $(BUILDROOTDIR)/* $(installdir)/www/

.PHONY: clean
clean:
	@echo "  clean..."
	@find -name "*.o" -delete
	@rm -rf $(TGT) $(OBJDIR) $(BUILDROOTDIR) doc/html *.gcno *.gcda *.gcov *~

.PHONY: indent
indent:
	./indent
