#ifndef MUD_NETWORK_PROTOCOL_H
#define MUD_NETWORK_PROTOCOL_H

/**
 * Typedefs
**/
typedef struct client client_t;
typedef struct protocol protocol_t;

typedef void (*protocol_func_t)(client_t*, void*);
typedef void (*protocol_data_func_t)(client_t*, void*, char*);
typedef void (*protocol_deallocate_func_t)(void*);

/**
 * Structs
**/
typedef struct protocol {
  void* data;
  protocol_func_t on_connected;
  protocol_data_func_t on_input;
  protocol_data_func_t on_output;
  protocol_deallocate_func_t on_deallocate;
  protocol_t* next;
} protocol_t;

/**
 * Function prototypes
**/
protocol_t* network_new_protocol_t();
void network_free_protocol_t(protocol_t* protocol);

#endif