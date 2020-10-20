CC=gcc
CFLAGS=-I../include -ggdb -Wall
LFLAGS=-lzlog -lpthread
BUILD_DIR=build
TARGET=mud

MODULES = network structure log
_OBJS = main.o game.o config.o mudstring.o network/network.o network/server.o network/client.o structure/node.o structure/list.o structure/queue.o structure/iterator.o log/log.o
OBJS = $(patsubst %,$(BUILD_DIR)/%,$(_OBJS))
OBJDIRS = $(patsubst %, $(BUILD_DIR)/%, $(MODULES))

all: $(OBJDIRS) $(TARGET)

$(TARGET): $(OBJS)
	gcc -o $@ $^ $(CFLAGS) $(LFLAGS)

$(BUILD_DIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIRS): $(BUILD_DIR)
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $@

install: all
	cp $(TARGET) ../dist

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
