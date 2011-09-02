INSTALL		= /usr/bin/install
installdir	:= $(ROOT)
bindir		:= $(installdir)/bin

export INSTALL
export bindir

tools := jsmin jsjuicer

.PHONY: all install clean mrproper
all install clean mrproper:
	@for dir in $(tools); do echo "  *** $@ $$dir"; $(MAKE) -C $$dir $@ || exit 1; done
