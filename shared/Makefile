# --------  --------  --------  --------  --------  --------  --------
# Shared code
# --------  --------  --------  --------  --------  --------  --------

include ../rules.mk

HEADERS = \
    timerInterrupt.h \
	threads.h \
	intercore.h \
	network.h \
	display.h

OBJS = \
    timerInterrupt.o \
	isr.o\
	threads.o \
	xfer.o \
	intercore.o \
	mq.o \
	enet.o \
	network.o \
	tcp.o \
	tftp.o \
	display.o \
	mcLibc.o \
	mcMain.o

all:	libmc.a

clean:
	$(RM) *.o *.lst *.s *~

libmc.a: $(OBJS)
	$(BAR) libmc.a $(OBJS)

$(OBJS): $(HEADERS)
