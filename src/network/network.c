#include "mud/network/network.h"
#include "mud/network/client.h"
#include "mud/network/server.h"
#include "mud/structure/list.h"
#include "mud/structure/node.h"

#include <zlog.h>
#include <stdlib.h>

network_t * network_new() {
    network_t * network = calloc(1, sizeof * network);

	network->server = network_server_new();
    network->clients = list_new();

    return network;

}

int network_initialise(network_t * network, int port) {
    zlog_category_t * networkCategory = zlog_get_category("network");

    network->server->port = port;

    if ( network_server_listen(network->server) == -1 ) {
        zlog_error(networkCategory, "network_initialise: Failed to listen on server.");

        return -1;
    }

    if ( network_server_create_thread(network->server, network->clients) == -1 ) {
        zlog_error(networkCategory, "network_initialise: Failed to create server accept thread.");

        return -1;
    }

	return 0;
}

int network_shutdown(network_t * network) {
    zlog_category_t * networkCategory = zlog_get_category("network");


    if ( !network ) {
        zlog_error(networkCategory, "network_shutdown: Network structure was null.");

        return -1;
    }

    if ( !network->server ) {
        zlog_error(networkCategory, "network_shutdown: Network server structure was null.");

        return -1;
    }

    network->server->shutdown = 1;

    if ( network_server_join_thread(network->server) == -1 ) {
        zlog_error(networkCategory, "network_shutdown: Network server join thread failed.");

        return -1;
    }

    if ( network_server_close(network->server) == -1 ) {
        zlog_error(networkCategory, "network_shutdown: Network server failed to close.");

        return -1;
    }

	return 0;
}

void network_free(network_t * network) {
    if ( network ) {
        if ( network->clients ) {
            node_t * node;

            for ( node = network->clients->first; node != network->clients->last; node = node->next ) {
                client_t * client = (client_t *)node->data;

                network_client_free(client);
            }

            list_free(network->clients);
        }

        free(network);
    }
}
