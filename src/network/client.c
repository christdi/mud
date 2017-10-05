#include "mud/network/client.h"
#include "mud/string.h"

#include <stdlib.h>
#include <zlog.h>

client_t * network_client_new() {
    client_t * client = calloc(1, sizeof * client);

    client->fd = 0;
    client->thread = 0;

    return client;
}

void network_client_free(client_t * client) {
    if ( client ) {
        free(client);
    }
}
