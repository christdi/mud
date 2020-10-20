#ifndef HG_NETWORK_H
#define HG_NETWORK_H

#include <sys/types.h>
#include <pthread.h>

#include "mud/structure/list.h"
#include "server.h"

/**
 * Sturcts
**/
typedef struct network {
	pthread_t thread;
	pthread_mutex_t lock;
	unsigned int shutdown;

	fd_set master_set;
	unsigned int max_fd;

	list_t * servers;
	list_t * clients;
} network_t;


/**
 * Functions prototypes
**/
network_t * create_network_t(void);
void free_network_t(network_t * network);

int initialise_network(network_t * network);
int shutdown_network(network_t * network);
int start_game_server(network_t * network, unsigned int port);
int stop_game_server(network_t * network, unsigned int port);


#endif
