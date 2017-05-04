#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <pthread.h>

struct server {
    unsigned int fd;
    pthread_t thread;
    unsigned int port;
    unsigned int backlog;
};

typedef struct server server;

server * network_server_new();
const int network_server_listen(server * server);
const int network_server_create_thread(server * server);
void * network_server_accept_thread(void * fd);
const int network_server_join_thread(server * server);
const int network_server_close(server * server);
void network_server_free(server * server);


#endif
