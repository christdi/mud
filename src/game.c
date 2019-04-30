#include "mud/game.h"
#include "mud/network/network.h"

#include <assert.h>
#include <stdlib.h>
#include <zlog.h>

game_t * game_new(void) {
    game_t * game = calloc(1, sizeof * game);

    game->shutdown = 0;
    game->network = 0;

    return game;
}

void game_free(game_t * game) {
    assert(game);
    assert(game->network);

    network_free(game->network);
    free(game);
}

int game_run(config_t * config) {
    zlog_category_t * gameCategory = zlog_get_category("game");

    zlog_info(gameCategory, "Starting MUD engine");

    game_t * game = game_new();
    gettimeofday(&game->last_tick, NULL);

    game->network = network_new();

	if ( network_initialise(game->network, 5000) == -1 ) {
		zlog_error(gameCategory, "Failed to initialise network");

		return -1;
	}

    long nanosecondsPerTick = 1000000000L / config->ticksPerSecond;

    while ( !game->shutdown ) {
        game_tick(game, nanosecondsPerTick);
        network_poll(game->network);
    }

	if ( network_shutdown(game->network) == -1 ) {
		zlog_error(gameCategory, "Failed to shutdown network");

		return -1;
	}

    game_free(game);

    zlog_info(gameCategory, "Stopping MUD engine");

    return 0;
}

int game_tick(game_t * game, const long nanosecondsPerTick) {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    time_t secondsElapsed = currentTime.tv_sec - game->last_tick.tv_sec;
    suseconds_t microsecondsElapsed = currentTime.tv_usec - game->last_tick.tv_usec;
    long nanosecondsElapsed = (secondsElapsed * 1000000000L) + (microsecondsElapsed * 1000);

    if (nanosecondsElapsed < nanosecondsPerTick) {
        struct timespec sleepTime;
        sleepTime.tv_sec = 0;
        sleepTime.tv_nsec = nanosecondsPerTick - nanosecondsElapsed;

        nanosleep(&sleepTime, NULL);
    }

    game->last_tick = currentTime;

    return 0;
}
