#include "mud/game.h"
#include "mud/network.h"

#include <zlog.h>

const int game_run() {
	zlog_category_t * gameCategory = zlog_get_category("game");

	server * server = network_server_new();
	server->port = 5000;

	network_server_listen(server);
	zlog_info(gameCategory, "Server successfully bound to port %d.", server->port);

	network_server_close(server);
	zlog_info(gameCategory, "Server on port %d successfully closed.", server->port);

	network_server_free(server);

	return 0;
}

const int game_bind_servers() {

}

const int game_close_servers() {
	
}
