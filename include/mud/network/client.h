#ifndef HG_CLIENT_H
#define HG_CLIENT_H

#include <pthread.h>

#include "mud/structure/queue.h"

typedef struct client {
    int fd;
    pthread_t thread;
    unsigned int hungup;

    queue_t * inputQueue;
} client_t;

client_t * network_client_new(void);
int network_client_send(client_t * client, char * data);
int network_client_create_thread(client_t * client);
void * network_client_receive_thread(void * receiveThreadData);
int network_client_join_thread(client_t * client);
int network_client_close(client_t * client);
void network_client_free(client_t * client);

#endif
