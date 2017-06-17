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
void network_client_free(client_t * client);

#endif
