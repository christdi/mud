CC=gcc
CFLAGS=-I. -I../include -ggdb
LFLAGS=-lzlog -lpthread
OBJECT_DIR=o
TARGET=mud

_OBJS = main.o game.o config.o string.o network.o
OBJS = $(patsubst %,$(OBJECT_DIR)/%,$(_OBJS))

$(OBJECT_DIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJS)
	gcc -o $@ $^ $(CFLAGS) $(LFLAGS)

install: $(TARGET)
	cp $^ ../dist

.PHONY: clean

clean:
	rm -f $(OBJECT_DIR)/*.o $(TARGET)
