#include "mud/game.h"
#include "mud/network/network.h"

#include <stdlib.h>
#include <zlog.h>

game_t * game_new() {
    game_t * game = calloc(1, sizeof * game);

    game->shutdown = 0;
    game->network = 0;

    return game;
}

void game_free(game_t * game) {
    if ( game ) {

        if ( game->network ) {
            network_free(game->network);
        }

        free(game);
    }
}

const int game_run() {
    zlog_category_t * gameCategory = zlog_get_category("game");

    zlog_info(gameCategory, "game_run: Starting MUD engine");

    game_t * game = game_new();
    game->network = network_new();

	zlog_info(gameCategory, "game_run: Initialising network");

	if ( network_initialise(game->network, 5000) == -1 ) {
		zlog_error(gameCategory, "game_run: Failed to initialise network");

		return -1;
	}

    while ( !game->shutdown ) {
    }

	if ( network_shutdown(game->network) == -1 ) {
		zlog_error(gameCategory, "game_run: Failed to shutdown network");

		return -1;
	}

    game_free(game);

    zlog_info(gameCategory, "game_run: Stopping MUD engine");

    return 0;
}

