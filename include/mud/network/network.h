#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <sys/types.h>
#include <pthread.h>

#include "mud/network/server.h"
#include "mud/network/callback.h"
#include "mud/structure/list.h"


/**
 * Sturcts
**/
typedef struct network {
	fd_set master_set;
	unsigned int max_fd;

	callback_t * connection_callback;
	callback_t * disconnection_callback;
	callback_t * input_callback;

	list_t * servers;
	list_t * clients;
} network_t;


/**
 * Functions prototypes
**/
network_t * create_network_t(void);
void free_network_t(network_t * network);

int start_game_server(network_t * network, unsigned int port);
int stop_game_server(network_t * network, unsigned int port);

void poll_network(network_t * network);
void disconnect_clients(network_t * network);

void register_connection_callback(network_t * network, callback_func func, void * context);
void register_disconnection_callback(network_t * network, callback_func func, void * context);
void register_input_callback(network_t * network, callback_func func, void * context);

#endif
