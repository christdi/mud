#include "mud/network/network.h"
#include "mud/network/client.h"
#include "mud/network/server.h"
#include "mud/structure/list.h"
#include "mud/structure/node.h"

#include <assert.h>
#include <zlog.h>
#include <stdlib.h>

network_t * network_new() {
    network_t * network = calloc(1, sizeof * network);

	network->server = network_server_new();

    return network;

}

const int network_initialise(network_t * network, int port) {
    assert(network);

    zlog_category_t * networkCategory = zlog_get_category("network");

    network->server->port = port;

    if ( network_server_listen(network->server) == -1 ) {
        zlog_error(networkCategory, "Failed to listen on server.");

        return -1;
    }

    if ( network_server_create_thread(network->server) == -1 ) {
        zlog_error(networkCategory, "Failed to create server accept thread.");

        return -1;
    }

	return 0;
}

const int network_poll(network_t * network) {
    assert(network);

    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( network_server_poll_clients(network->server) == -1 ) {
        zlog_error(networkCategory, "Failed to poll server.");

        return -1;
    }

    return 0;
}

const int network_shutdown(network_t * network) {
    assert(network);

    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( network_server_shutdown(network->server) == -1 ) {
        zlog_error(networkCategory, "Network server failed to shutdown.");

        return -1;
    }

	return 0;
}

void network_free(network_t * network) {
    assert(network);
    assert(network->server);

    
    network_server_free(network->server);
    free(network);
}
