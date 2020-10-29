CC=gcc
CFLAGS=-I../include -ggdb -Wall
LFLAGS=-lzlog -lpthread -luuid
TARGET=mud

OBJS = main.o game.o config.o player.o $(DATA_OBJS) $(NETWORK_OBJS) $(ENTITY_OBJS) \
$(LOG_OBJS) $(NETWORK_OBJS) $(STATE_OBJS) $(UTIL_OBJS)

DATA_OBJS = data/hash_table/hash_table.o data/hash_table/hash_node.o \
data/hash_table/hash_iterator.o data/linked_list/linked_list.o data/linked_list/node.o \
data/linked_list/iterator.o data/queue/queue.o

ENTITY_OBJS = entity/components.o entity/character_details.o entity/entity.o

LOG_OBJS = log/log.o

NETWORK_OBJS = network/network.o network/server.o network/client.o network/callback.o

STATE_OBJS = state/login_state.o

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
