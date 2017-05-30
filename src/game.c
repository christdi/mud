#include "mud/game.h"
#include "mud/network/server.h"

#include <stdlib.h>
#include <zlog.h>

game_t * game_new() {
    game_t * game = calloc(1, sizeof * game);

    game->shutdown = 0;
    game->server = 0;

    return game;
}

void game_free(game_t * game) {
    if ( game ) {

        if ( game->server ) {
            network_server_free(game->server);
        }

        free(game);
    }
}

const int game_run() {
    zlog_category_t * gameCategory = zlog_get_category("game");

    zlog_info(gameCategory, "game_run: Starting MUD engine");

    game_t * game = game_new();
    game->server = network_server_new();
    game->server->port = 5000;

    if ( network_server_listen(game->server) != 0 ) {
        zlog_error(gameCategory, "game_run: Failed to bind server.");

        return -1;
    }

    if ( network_server_create_thread(game->server) != 0 ) {
        zlog_error(gameCategory, "game_run: Failed to create server thread.");

        return -1;
    }

    zlog_info(gameCategory, "game_run: Server successfully bound to port %d.", game->server->port);

    while ( !game->shutdown ) {
        game->shutdown = 1;
    }

    if ( network_server_join_thread(game->server) != 0 ) {
        zlog_error(gameCategory, "game_run: Failed to join server thread.");

        return -1;
    }

    if ( network_server_close(game->server) != 0 ) {
        zlog_error(gameCategory, "game_run: Failed to close server.");

        return -1;
    }

    zlog_info(gameCategory, "game_run: Server on port %d successfully closed.", game->server->port);

    game_free(game);

    zlog_info(gameCategory, "game_run: Stopping MUD engine");

    return 0;
}

