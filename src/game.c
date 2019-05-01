#include "mud/game.h"
#include "mud/log/log.h"
#include "mud/network/network.h"

#include <assert.h>
#include <stdlib.h>
#include <zlog.h>


void game_tick(game_t * game, unsigned int ticksPerSecond);


/**
 * Allocate a new instance of a game_t struct.
 *
 * Returns an allocated game_t struct with default values.
**/
game_t * game_new(void) {
  game_t * game = calloc(1, sizeof * game);

  game->shutdown = 0;
  game->network = 0;

  return game;
}


/**
 * Frees an allocated game_t struct.
**/
void game_free(game_t * game) {
  assert(game);
  assert(game->network);

  free_network_t(game->network);
  free(game);
}


/**
 * Starts running the game, binds a server and enters the game loop.
 *
 * Returns a 0 on success or -1 on failure.
**/
int start_game(config_t * config) {
  zlog_info(gc, "Starting MUD engine");

  game_t * game = game_new();
  gettimeofday(&game->lastTick, NULL);

  game->network = create_network_t();

  if (initialise_network(game->network) != 0) {
    zlog_error(gc, "Failed to initialise network");

    return -1;
  }

  if (start_game_server(game->network, 5000) == -1) {
    zlog_error(gc, "Failed to start game server");

    return -1;
  }

  while (!game->shutdown) {
    game_tick(game, config->ticksPerSecond);
  }

  if (stop_game_server(game->network, 5000) == -1) {
    zlog_error(gc, "Failed to shutdown server");

    return -1;
  }

  if (shutdown_network(game->network) != 0) {
    zlog_error(gc, "Failed to shutdown network");

    return -1;
  }

  game_free(game);

  zlog_info(gc, "Stopping MUD engine");

  return 0;
}


/**
 * Forces the game loop to adhere to a spcified ticks per second.  Calculates the elapsed time
 * time since the last time the method was called and makes the thread sleep if it's less than
 * the amount of time calculated per tick.
**/
void game_tick(game_t * game, const unsigned int ticksPerSecond) {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);

  time_t secondsElapsed = currentTime.tv_sec - game->lastTick.tv_sec;
  suseconds_t microsecondsElapsed = currentTime.tv_usec - game->lastTick.tv_usec;
  long nanosecondsElapsed = (secondsElapsed * 1000000000L) + (microsecondsElapsed * 1000);
  long nanosecondsPerTick = 1000000000L / ticksPerSecond;

  if (nanosecondsElapsed < nanosecondsPerTick) {
    struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = nanosecondsPerTick - nanosecondsElapsed;

    nanosleep(&sleepTime, NULL);
  }

  game->lastTick = currentTime;
}
