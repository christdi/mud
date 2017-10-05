#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "mud/list/list.h"
#include "server.h"

struct network {
	server_t * server;
    list_t * clients;
};

typedef struct network network_t;

network_t * network_new();
int network_initialise(network_t * network, int port);
int network_shutdown(network_t * network);
void network_free(network_t * network);

#endif
