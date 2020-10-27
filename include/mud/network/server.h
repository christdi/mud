#ifndef _SERVER_H_
#define _SERVER_H_

#include "mud/data/linked_list/linked_list.h"
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
