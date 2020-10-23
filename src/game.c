#include "mud/game.h"
#include "mud/log/log.h"
#include "mud/event/event.h"
#include "mud/network/network.h"
#include "mud/structure/queue.h"

#include <assert.h>
#include <stdlib.h>
#include <zlog.h>


void game_tick(game_t * game, unsigned int ticksPerSecond);


/**
 * Allocate a new instance of a game_t struct.
 *
 * Returns an allocated game_t struct with default values.
**/
game_t * create_game_t(void) {
  game_t * game = calloc(1, sizeof * game);

  game->shutdown = 0;
  gettimeofday(&game->last_tick, NULL);

  game->network = create_network_t();
  game->components = create_components_t();
  game->events = create_list_t();

  return game;
}


/**
 * Frees an allocated game_t struct.
**/
void free_game_t(game_t * game) {
  assert(game);
  assert(game->network);
  assert(game->components);

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
  zlog_info(gc, "Starting MUD engine");

  if (initialise_network(game->network) != 0) {
    zlog_error(gc, "Failed to initialise network");

    return -1;
  }

  if (start_game_server(game->network, 5000) == -1) {
    zlog_error(gc, "Failed to start game server");

    return -1;
  }

  while (!game->shutdown) {
    event_t * event;

    while ((event = queue_dequeue(game->events)) != NULL) {
      // TODO: Handle event
    }

    game_tick(game, config->ticks_per_second);
  }

  if (stop_game_server(game->network, 5000) == -1) {
    zlog_error(gc, "Failed to shutdown server");

    return -1;
  }

  if (shutdown_network(game->network) != 0) {
    zlog_error(gc, "Failed to shutdown network");

    return -1;
  }

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
