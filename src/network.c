#include "mud/network.h"

#include <stdlib.h>

server * network_server_new() {
	server * server = calloc(1, sizeof * server);

	server->port = 0;

	return server;
}

const int network_server_listen(server * server) {
	return 0;

}

const int network_server_close(server * server) {
	return 0;
}

void network_server_free(server * server) {
	if ( server ) {
		free(server);
	}
}