#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "mud/structure/list.h"
#include "server.h"

struct network {
	server_t * server;
};

typedef struct network network_t;

network_t * network_new();
const int network_initialise(network_t * network, int port);
const int network_poll(network_t * network);
const int network_shutdown(network_t * network);
void network_free(network_t * network);

#endif
