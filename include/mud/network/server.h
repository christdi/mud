#ifndef _SERVER_H_
#define _SERVER_H_

#include <uv.h>

/**
 * Defines
 **/
#define PORT_SIZE 15

/**
 * Typedefs
 **/
typedef struct network network_t;

/**
 * Structs
 **/
typedef struct server {
  uv_tcp_t handle;
  unsigned int port;
  unsigned int backlog;
  network_t* network;
} server_t;

/**
 * Function prototypes
 **/
server_t* create_server_t(void);
void free_server_t(server_t* server);

#endif
