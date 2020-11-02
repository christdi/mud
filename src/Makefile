CC=gcc
CFLAGS=-I../include -ggdb -Wall
LFLAGS=-lzlog -lpthread
TARGET=mud

OBJS = main.o game.o config.o player.o $(COMMAND_OBJS) $(DATA_OBJS) $(NETWORK_OBJS) \
$(ECS_OBJS) $(LOG_OBJS) $(NETWORK_OBJS) $(STATE_OBJS) $(UTIL_OBJS)

COMMAND_OBJS = command/command.o command/communication.o command/general.o

DATA_OBJS = data/hash_table/hash_table.o data/hash_table/hash_node.o \
data/hash_table/hash_iterator.o data/linked_list/linked_list.o data/linked_list/node.o \
data/linked_list/iterator.o data/queue/queue.o

ECS_OBJS = ecs/components.o ecs/character_details.o ecs/entity.o ecs/system.o

LOG_OBJS = log/log.o

NETWORK_OBJS = network/network.o network/server.o network/client.o network/callback.o

STATE_OBJS = state/login_state.o state/play_state.o

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
