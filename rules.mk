# --------  --------  --------  --------  --------  --------  --------
# General-purpose stuff
# --------  --------  --------  --------  --------  --------  --------
APIARY=/opt/apiary
HEADERS = \
	shared/setjmp.h \
	shared/threads.h \
	shared/intercore.h \
	shared/counters.h \
	shared/monitor.h \
	shared/tm.h \
	shared/reboot.h \
	shared/network.h \
	shared/display.h \
	shared/mtd.h

BASE = $(APIARY)/lib/base.o

LIBMC = shared/libmc.a

INCLUDE = $(APIARY)/include

CARGS =

BDATA = Bfiledata
BCC = Bgcc1 -quiet -std=c99 -fno-builtin -msavertn -O2 \
	-Ishared -I$(INCLUDE) $(CARGS)
BAS = Bas -datarota=2 -I$(INCLUDE)
BAR = Bar r
BLD = Bld -codebase=1000 -datafloat $(BASE)
LIBS = -Lshared -lmc -L$(APIARY)/lib -lc -lgcc
BIMG = Bimg
BSIM = Bsim

RM = rm
COPY = copy
TOUCH = touch

%.o:	%.c # cancel built-in rules

%.c %.h: %.jpg
	$(BDATA) $<

%.c %.h: %.bmp
	$(BDATA) $<

%.c %.h:	%.gif
	$(BDATA) $<

%.c %.h: %.html
	$(BDATA) $<

%.s:	%.c
	$(BCC) $<

%.o:	%.s
	$(BAS) -lst $<.lst -x $<

%.o:	%.as
	$(BAS) -lst $<.lst -x $<

%.img:	%.out
	$(BIMG) $<

.PHONY: all tidy clean
