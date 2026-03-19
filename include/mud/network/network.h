#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <uv.h>

#include "mud/network/callback.h"

/**
 * Definitions
 **/
#define BACKLOG 10

/**
 * Typedefs
 **/
typedef struct linked_list linked_list_t;

/**
 * Structs
 **/
typedef struct network {
  uv_loop_t* loop;

  callback_t* connection_callback;
  callback_t* disconnection_callback;
  callback_t* input_callback;
  callback_t* flush_callback;

  linked_list_t* servers;
  linked_list_t* clients;
} network_t;

/**
 * Function prototypes
 **/
network_t* create_network_t(void);
void free_network_t(network_t* network);

int start_game_server(network_t* network, unsigned int port);
int stop_game_server(network_t* network, unsigned int port);

void flush_output(network_t* network);
void disconnect_clients(network_t* network);
void network_shutdown(network_t* network);

void register_connection_callback(network_t* network, callback_func func, void* context);
void register_disconnection_callback(network_t* network, callback_func func, void* context);
void register_input_callback(network_t* network, callback_func func, void* context);
void register_flush_callback(network_t* network, callback_func func, void* context);

#endif
