#include "mud/game.h"
#include "mud/network/network.h"

#include <assert.h>
#include <stdlib.h>
#include <zlog.h>

game_t * game_new() {
    game_t * game = calloc(1, sizeof * game);

    game->shutdown = 0;
    game->network = 0;

    return game;
}

void game_free(game_t * game) {
    assert(game);
    assert(game->network);

    free(game);
    network_free(game->network);
}

const int game_run() {
    zlog_category_t * gameCategory = zlog_get_category("game");

    zlog_info(gameCategory, "Starting MUD engine");

    game_t * game = game_new();
    gettimeofday(&game->last_tick, NULL);

    game->network = network_new();

	if ( network_initialise(game->network, 5000) == -1 ) {
		zlog_error(gameCategory, "Failed to initialise network");

		return -1;
	}

    while ( !game->shutdown ) {
        game_tick(game);
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

const int game_tick(game_t * game) {
    gettimeofday(&game->last_tick, NULL);

    return 0;
}
