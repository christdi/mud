CC=gcc
CFLAGS=-I../include -ggdb -Wall
LFLAGS=-lzlog -lpthread
TARGET=mud

OBJS = main.o game.o config.o player.o $(NETWORK_OBJS) $(DATA_OBJS) \
$(LOG_OBJS) $(ENTITY_OBJS) $(UTIL_OBJS)

NETWORK_OBJS = network/network.o network/server.o network/client.o network/callback.o

DATA_OBJS = data/hash_table/hash_table.o data/hash_table/hash_node.o \
data/linked_list/linked_list.o data/linked_list/node.o data/linked_list/iterator.o \
data/queue_linked_list/queue.o

LOG_OBJS = log/log.o

ENTITY_OBJS = entity/components.o entity/character_details.o entity/entity.o

UTIL_OBJS = util/mudstring.o util/muduuid.o

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
