#ifndef _SERVER_H_
#define _SERVER_H_

#include "mud/structure/list.h"

#include <pthread.h>


typedef struct server server_t;
typedef struct server_thread_data server_thread_data_t;

struct server {
    unsigned int fd;
    pthread_t thread;
    unsigned int port;
    unsigned int backlog;
    unsigned int shutdown;
};

struct server_thread_data {
	server_t * server;
	list_t * clients;
};


server_t * network_server_new();
const int network_server_listen(server_t * server);
const int network_server_create_thread(server_t * server, list_t * clients);
void * network_server_accept_thread(void * serverThreadData);
const int network_server_join_thread(server_t * server);
const int network_server_close(server_t * server);
void network_server_free(server_t * server);

server_thread_data_t * network_server_thread_data_new();
void network_server_thread_data_free(server_thread_data_t * serverThreadData);

#endif
