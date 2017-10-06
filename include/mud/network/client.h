#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>

struct client {
    unsigned int fd;
    pthread_t thread;
    unsigned int shutdown;
};

typedef struct client client_t;

client_t * network_client_new();
const int network_client_send(client_t * client, char * data);
void network_client_free(client_t * client);

#endif
