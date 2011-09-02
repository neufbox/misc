# Makefile

# compilation variables
LDLIBS	= -Wl,-Bstatic -lewf $(LIBSSTA) -Wl,-Bdynamic $(LIBSDYN)

CFLAGS	+= -I../include/
LDFLAGS += -L../

############################################################
# list test target
tests  = test_session test_crypto test_theme

# rules
.PHONY: all
all: build

.PHONY: build
build: $(tests)

test_crypto: test_crypto.o
test_session: test_session.o
test_theme: test_theme.o

.PHONY: install
install: build
	$(INSTALL) -d $(bindir)/
	$(INSTALL) -d $(bindir)/ewf/
	$(INSTALL) -m 755 $(tests) $(bindir)/ewf/

clean:
	find -name "*.o" -delete
	rm -rf $(tests)
