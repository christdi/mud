#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>

#include "mud/structure/queue.h"

struct client {
    unsigned int fd;
    pthread_t thread;
    unsigned int hungup;

    queue_t * inputQueue;

};

typedef struct client client_t;

client_t * network_client_new();
const int network_client_send(client_t * client, char * data);
const int network_client_create_thread(client_t * client);
void * network_client_receive_thread(void * receiveThreadData);
const int network_client_join_thread(client_t * client);
const int network_client_close(client_t * client);
void network_client_free(client_t * client);

#endif
