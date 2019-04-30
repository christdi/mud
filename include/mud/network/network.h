#ifndef HG_NETWORK_H
#define HG_NETWORK_H

#include "mud/structure/list.h"
#include "server.h"

typedef struct network {
	server_t * server;
} network_t;


network_t * network_new(void);
int network_initialise(network_t * network, unsigned int port);
int network_poll(network_t * network);
int network_shutdown(network_t * network);
void network_free(network_t * network);

#endif
