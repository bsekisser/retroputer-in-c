CFLAGS = -g -Wall
CFLAGS += -Os
CFLAGS += -MMD -MP
CFLAGS += $(INCLUDES)

INCLUDES = -Iinclude
INCLUDES += -Icore/include
INCLUDES += -I../../include

TARGET = main

VPATH = ../../include

all: $(TARGET)

clean: libcore-clean
	-rm -f *.d *.o $(TARGET)

$(TARGET): main.o core/libcore.a callback_qlist.o

core/libcore.a:
	$(MAKE) -C core

libcore-clean:
	$(MAKE) -C core clean

-include *.d
