
CFLAGS += -std=gnu99

all: mac_addr iomem ledctl spy-send

mac_addr: mac_addr.o

iomem: iomem.o

ledctl: ledctl.o

spy-send: LDLIBS += -letk
spy-send: spy-send.o

.PHONY: indent
indent:
	find -name "*.[c|h]" -exec indent -linux -cp1 -il0 {} \;
	find -name "*~" -delete
