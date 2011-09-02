# Makefile

# target config
TARGET		:= libnbu
TARGET_SHARED	:= $(TARGET).so
TARGET_STATIC	:= $(TARGET).a

TEST_TARGET     :=

# global variables
INSTALL		:= /usr/bin/install
RM              := rm -rf

# compilation variables
CFLAGS		:= -I./include $(CFLAGS) -D_GNU_SOURCE -std=c99 -fPIC

ifdef DEBUG
CFLAGS		+= -g
CFLAGS		+= -Wall -Wextra -Wbad-function-cast -Wshadow \
                   -Wcast-qual -Wold-style-definition -Wmissing-noreturn \
                   -Wstrict-prototypes -Waggregate-return -Wformat=2 \
                   -Wswitch-default -Wundef -Wbad-function-cast \
                   -Wunused-parameter -Wnonnull
LDFLAGS		:= -L./ $(LDFLAGS)
TEST_TARGET	+= $(foreach testfile, $(wildcard tests/*.c), tests/test-$(notdir $(basename $(testfile))))
else
CFLAGS		+= -Os -DNDEBUG
LDFLAGS		+= -s
endif

# installation variables
prefix		:= /usr
libdir		:= $(prefix)/lib
includedir	:= $(prefix)/include

############################################################

# tgts
tgts		:= $(TARGET_SHARED) $(TARGET_STATIC)
extra_tgts	:= $(TEST_TARGET)

# list C files
sources		:= $(wildcard src/*.c)
test_sources	:= $(wildcard tests/*.c)

# get objects from C files
objects		:= $(sources:.c=.o)
test_objects	:= $(test_sources:.c=.o)

# rules
.PHONY: all
all: $(tgts) $(extra_tgts)
	@echo "**********************************************"
	@echo "*            COMPILATION FINISH              *"
	@echo "*                                            *"
	@echo "* TGTS:                                      *"
	@for tgt in $^; \
	do \
		echo "*   - $$tgt"; \
	done;
	@echo "* DEBUG=$(DEBUG)                                    *"
	@echo "**********************************************"

%.o: %.c
	@echo "  cc $<"
	@${CC} ${CFLAGS} -c $< -o $@

$(TARGET_SHARED): $(objects)
	@echo "  LD $@"
	@$(CC) $(LDFLAGS) -shared $^ $(LDLIBS) -o $@

$(TARGET_STATIC): $(objects)
	@echo "  AR $@"
	@$(AR) rcs $@ $^

tests/test-%: tests/%.o
	@echo "  LD $@"
	@$(CC) $^ $(LDFLAGS) $(LDLIBS) -ldmalloc -lnbu -o $@

tests/%.o: tests/%.c
	@echo "  cc $<"
	@$(CC) ${CFLAGS} -c $< -o $@

.PHONY: clean
clean:
	find -name "*.o" -delete
	$(RM) $(TARGET_SHARED) $(TARGET_STATIC) $(extra_tgts)

.PHONY: mrproper
mrproper: clean

.PHONY: install
install: all
	@echo "   INSTALL $(TARGET_SHARED)"
	@$(INSTALL) -d $(libdir)/
	@$(INSTALL) -m 755 $(TARGET_SHARED) $(libdir)/$(TARGET_SHARED)
	@$(INSTALL) -m 755 $(TARGET_STATIC) $(libdir)/$(TARGET_STATIC)
	@$(INSTALL) -d $(includedir)/nbu
	@$(INSTALL) -m 644 include/nbu/*.h $(includedir)/nbu 

.PHONY: uninstall
uninstall:
	@echo "   UNINSTALL $(TARGET_SHARED)"
	$(RM) $(libdir)/$(TARGET_SHARED) $(libdir)/$(TARGET_STATIC)
