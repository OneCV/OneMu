CC=gcc
SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)

INCLUDE=-I$(PWD)/../../../mugadget/include/
INCLUDE+=-I$(PWD)/../../../mucore/include/
LIBS=$(PWD)/../../../prebuilt/Linux/x86_64/libOneMuGadget.a \
	 $(PWD)/../../../prebuilt/Linux/x86_64/libOneMu.a

CFLAGS= $(INCLUDE) -g -Wall -O2 -lm 
TARGET= helloMu

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o
	rm -rf $(TARGET)
