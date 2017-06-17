#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "mud/list/list.h"
#include "server.h"

struct network {
    list_t * clients;
};

typedef struct network network_t;

network_t * network_new();
void network_free(network_t * network);

#endif
