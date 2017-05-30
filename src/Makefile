CC=gcc
CFLAGS=-I. -I../include -ggdb
LFLAGS=-lzlog -lpthread
BUILD_DIR=build
TARGET=mud

MODULES = network
_OBJS = main.o game.o config.o string.o network/server.o network/client.o
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

install: $(TARGET)
	cp $^ ../dist

.PHONY: clean

clean:
	rm -rf $(OBJDIRS) $(BUILD_DIR)/*.o $(TARGET)
