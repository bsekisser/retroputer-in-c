CFLAGS = -g -Wall
CFLAGS += -MD -MP
CFLAGS += -O2
CFLAGS += $(INCLUDES)

INCLUDES = -Iinclude
INCLUDES += -I../../include

VPATH = ../../include

#TARGET = retroputer.exe
TARGET = main

.PRECIOUS: %.o

all: $(TARGET)

clean:
	-rm *.d *.o

clean-all: clean
	-rm *.a $(TARGET)

libcore.a: libcore.a(core.o core_trace.o libmemory.a)

libio.a: libio.a(io_buss.o core_trace.o)

libmemory.a: libmemory.a(memory.o kernel_data.o)

$(TARGET): libcore.a libio.a libmemory.a callback_qlist.o main.o
