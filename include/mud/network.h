#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <pthread.h>

struct server {
    unsigned int fd;
    pthread_t thread;
    unsigned int port;
    unsigned int backlog;
};

typedef struct server server_t;

server_t * network_server_new();
const int network_server_listen(server_t * server);
const int network_server_create_thread(server_t * server);
void * network_server_accept_thread(void * fd);
const int network_server_join_thread(server_t * server);
const int network_server_close(server_t * server);
void network_server_free(server_t * server);


#endif
