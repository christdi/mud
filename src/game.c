#include "mud/game.h"
#include "mud/network.h"

#include <zlog.h>

const int game_run() {
    zlog_category_t * gameCategory = zlog_get_category("game");

    zlog_info(gameCategory, "game_run: Starting MUD engine");

    server * server = network_server_new();
    server->port = 5000;

    if ( network_server_listen(server) != 0 ) {
        zlog_error(gameCategory, "game_run: Failed to bind server.");

        return -1;
    }

    if ( network_server_create_thread(server) != 0 ) {
        zlog_error(gameCategory, "game_run: Failed to create server thread.");

        return -1;
    }

    zlog_info(gameCategory, "game_run: Server successfully bound to port %d.", server->port);

    if ( network_server_join_thread(server) != 0 ) {
        zlog_error(gameCategory, "game_run: Failed to join server thread.");

        return -1;
    }

    if ( network_server_close(server) != 0 ) {
        zlog_error(gameCategory, "game_run: Failed to close server.");

        return -1;
    }

    zlog_info(gameCategory, "game_run: Server on port %d successfully closed.", server->port);

    network_server_free(server);

    zlog_info(gameCategory, "game_run: Stopping MUD engine");

    return 0;
}

const int game_bind_servers() {

}

const int game_close_servers() {

}
