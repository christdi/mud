#include "mud/network/network.h"
#include "mud/network/client.h"
#include "mud/list/list.h"
#include "mud/list/node.h"

#include <stdlib.h>

network_t * network_new() {
    network_t * network = calloc(1, sizeof * network);

    network->clients = list_new();

    return network;

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
