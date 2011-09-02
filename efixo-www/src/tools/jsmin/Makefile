TGT=jsmin

all: $(TGT)

$(TGT): jsmin.c

mrproper clean:
	rm -f $(TGT)

install:
	$(INSTALL) -m 755 $(TGT) $(bindir)/