CC=gcc
CFLAGS=-I../include -ggdb -Wall
LFLAGS=-lzlog -lpthread -luuid
TARGET=mud

OBJS = main.o game.o config.o player.o network/network.o network/server.o \
network/client.o network/callback.o structure/node.o structure/list.o \
structure/queue.o structure/iterator.o structure/hash_table.o structure/hash_node.o \
log/log.o entity/components.o entity/character_details.o entity/entity.o event/event.o \
util/mudstring.o util/muduuid.o

all: $(TARGET)

$(TARGET): $(OBJS)
	${CC} -o $@ $^ $(CFLAGS) $(LFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

install: all
	cp $(TARGET) ../dist

.PHONY: clean

clean:
	rm -rf $(OBJS) $(TARGET)
