#ifndef _SERVER_H_
#define _SERVER_H_

#include "mud/structure/list.h"

#include <pthread.h>


typedef struct server server_t;

struct server {
    unsigned int fd;
    pthread_t thread;
    unsigned int port;
    unsigned int backlog;

    list_t * clients;
};

server_t * network_server_new();
const int network_server_initialise(server_t * server);
const int network_server_listen(server_t * server);
const int network_server_create_thread(server_t * server);
void * network_server_accept_thread(void * serverThreadData);
const int network_server_poll_clients(server_t * server);
const int network_server_join_thread(server_t * server);
const int network_server_close(server_t * server);
const int network_server_shutdown(server_t * server);
void network_server_free(server_t * server);

#endif
