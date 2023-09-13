CFLAGS = -g -Wall
CFLAGS += -O2
CFLAGS += -MMD -MP
CFLAGS += $(INCLUDES)

INCLUDES = -Iinclude
INCLUDES += -I../include
INCLUDES += -I../../../include

VPATH = ../../../include

all: libcore.a

clean:
	-rm libcore.a *.o *.d

.PRECIOUS: %.o

libcore.a: libcore.a(alu.o buss.o memory.o memory_bank.o system_buss.o)
libcore.a: callback_qlist.o

-include *.d
