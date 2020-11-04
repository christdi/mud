CC=gcc
CFLAGS=-I../include -ggdb -Wall
LFLAGS=-lzlog -lpthread
TARGET=mud

OBJS = main.o game.o config.o player.o log.o $(ACTION_OBJS) $(COMMAND_OBJS) $(DATA_OBJS) \
$(NARRATOR_OBJS) $(NETWORK_OBJS) $(ECS_OBJS) $(NETWORK_OBJS) $(STATE_OBJS) $(UTIL_OBJS)

ACTION_OBJS = action/action.o action/action_callback.o

COMMAND_OBJS = command/admin.o command/command.o command/communication.o command/explore.o command/general.o

DATA_OBJS = data/hash_table/hash_table.o data/hash_table/hash_node.o \
data/hash_table/hash_iterator.o data/linked_list/linked_list.o data/linked_list/node.o \
data/linked_list/iterator.o data/queue/queue.o

ECS_OBJS = ecs/components.o ecs/contained.o ecs/container.o ecs/description.o ecs/entity.o ecs/system.o

LOG_OBJS = log/log.o

NARRATOR_OBJS = narrator/narrator.o

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
