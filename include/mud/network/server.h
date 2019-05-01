#ifndef HG_SERVER_H
#define HG_SERVER_H

#include "mud/structure/list.h"
#include "mud/network/client.h"

#include <pthread.h>

/**
 * Structs
**/
typedef struct server {
    unsigned int fd;  
    unsigned int port;
    unsigned int backlog;
} server_t;

/**
 * Function prototypes
**/
server_t * create_server_t(void);
void free_server_t(server_t * server);

int listen_on_server(server_t * server);
int accept_on_server(server_t * server, client_t * client);
int close_server(server_t * server);

#endif
