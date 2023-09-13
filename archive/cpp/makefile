CFLAGS = -g -Wall
CFLAGS += -Os
CFLAGS += -MMD -MP
CFLAGS += $(INCLUDES)

CPPFLAGS = $(CFLAGS)

INCLUDES = -Iinclude
INCLUDES += -I../../../include

OBJS = alu.o \
	buss.o \
	buss_receiver.o \
	memory.o \
	memory_bank.o \
	system_buss.o

TARGET = wtf

VPATH = ../../include

all: $(OBJS)

clean:
	-rm -f *.d *.o $(TARGET)

$(TARGET): main.o core/libcore.a callback_qlist.o


-include *.d
