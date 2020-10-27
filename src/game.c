#include "mud/game.h"
#include "mud/player.h"
#include "mud/log/log.h"
#include "mud/network/network.h"
#include "mud/data/queue_linked_list/queue.h"
#include "mud/util/mudstring.h"

#include <assert.h>
#include <stdlib.h>
#include <zlog.h>
#include <string.h>


void game_tick(game_t * game, unsigned int ticks_per_second);
void player_connected(client_t * client, void * context);
void player_disconnected(client_t * client, void * context);
void player_input(client_t * client, void * context);


/**
 * Allocate a new instance of a game_t struct.
 *
 * Returns an allocated game_t struct with default values.
**/
game_t * create_game_t(void) {
  game_t * game = calloc(1, sizeof * game);

  game->shutdown = 0;
  gettimeofday(&game->last_tick, NULL);

  game->players = create_hash_table_t();

  game->network = create_network_t();
  game->components = create_components_t();

  return game;
}


/**
 * Frees an allocated game_t struct.
**/
void free_game_t(game_t * game) {
  assert(game);
  assert(game->players);
  assert(game->network);
  assert(game->components);

  free_hash_table_t(game->players);
  free_network_t(game->network);
  free_components_t(game->components);
  free(game);
}




/**
 * Starts running the game, binds a server and enters the game loop.
 *
 * Returns a 0 on success or -1 on failure.
**/
int start_game(game_t * game, config_t * config) {
  assert(game);
  assert(config);

  zlog_info(gc, "Starting MUD engine");

  register_connection_callback(game->network, player_connected, game);
  register_disconnection_callback(game->network, player_disconnected, game);
  register_input_callback(game->network, player_input, game);

  if (start_game_server(game->network, 5000) == -1) {
    zlog_error(gc, "Failed to start game server");

    return -1;
  }

  while (!game->shutdown) {
    poll_network(game->network);

    game_tick(game, config->ticks_per_second);
  }

  if (stop_game_server(game->network, 5000) == -1) {
    zlog_error(gc, "Failed to shutdown server");

    return -1;
  }

  disconnect_clients(game->network);

  zlog_info(gc, "Stopping MUD engine");

  return 0;
}
  

/**
 * Forces the game loop to adhere to a spcified ticks per second.  Calculates the elapsed time
 * time since the last time the method was called and makes the thread sleep if it's less than
 * the amount of time calculated per tick.
**/
void game_tick(game_t * game, const unsigned int ticks_per_second) {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);

  time_t seconds_elapsed = current_time.tv_sec - game->last_tick.tv_sec;
  suseconds_t microseconds_elapsed = current_time.tv_usec - game->last_tick.tv_usec;
  long nanoseconds_elapsed = (seconds_elapsed * 1000000000L) + (microseconds_elapsed * 1000L);
  long nanoseconds_per_tick = 1000000000L / ticks_per_second;

  if (nanoseconds_elapsed < nanoseconds_per_tick) {
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = nanoseconds_per_tick - nanoseconds_elapsed;

    nanosleep(&sleep_time, NULL);
  }

  game->last_tick = current_time;
}


/**
 * Callback from the network module when a new client connects.
**/
void player_connected(client_t * client, void * context) {
  game_t * game = (game_t *) context;

  player_t * player = create_player_t();
  player->client = client;

  send_to_player(player, "Welcome player, your uuid is [%s]\n\r", client->uuid);

  hash_table_insert(game->players, client->uuid, player);
}


/**
 * Callback from the network module when a client disconnects.
**/
void player_disconnected(client_t * client, void * context) {
  game_t * game = (game_t *) context;

  player_t * player = hash_table_delete(game->players, client->uuid);
  free_player_t(player);
}


/**
 * Callback from the network module when a client receives input.
**/
void player_input(client_t * client, void * context) {
  game_t * game = (game_t *) context;

  player_t * player = hash_table_get(game->players, client->uuid);

  char command[20];

  if (extract_from_input(client, command, 20, "\r\n") != -1 ) {
    if (strncmp(command, "quit", 4) == 0) {
      client->hungup = 1;
    }

    if (strncmp(command, "shutdown", 8) == 0) {
      game->shutdown = 1;
    }
  };

  
}