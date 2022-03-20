#ifndef MUD_NETWORK_PROTOCOL_H
#define MUD_NETWORK_PROTOCOL_H

/**
 * Typedefs
**/
typedef struct client client_t;
typedef struct protocol protocol_t;

typedef void (*protocol_func_t)(client_t*, void*);
typedef int (*protocol_data_func_t)(client_t*, void*, char*, size_t);
typedef void (*protocol_flush_func_t)(client_t*, void*, char*, size_t);
typedef void (*protocol_deallocate_func_t)(void*);

/**
 * Enums
**/
typedef enum protocol_type {
  TELNET
} protocol_type_t;

/**
 * Structs
**/
typedef struct protocol {
  void* data;

  protocol_type_t type;

  protocol_deallocate_func_t deallocator;

  protocol_func_t initialiser;
  protocol_data_func_t on_input;
  protocol_data_func_t on_output;
  protocol_flush_func_t on_flush;

  protocol_t* next;
} protocol_t;

/**
 * Function prototypes
**/
protocol_t* network_new_protocol_t();
void network_free_protocol_t(protocol_t* protocol);
void network_deallocate_protocol_chain(protocol_t* protocol);

void network_protocol_chain_initialise(client_t* client);
int network_protocol_chain_on_input(client_t* client, char* input, size_t len);
int network_protocol_chain_on_output(client_t* client, char* output, size_t len);
void network_protocol_chain_on_flush(client_t* client, char* output, size_t len);

void network_protocol_initialise(protocol_t* protocol, client_t* client);
int network_protocol_on_input(protocol_t* protocol, client_t* client, char* input, size_t len);
int network_protocol_on_output(protocol_t* protocol, client_t* client, char* output, size_t len);
void network_protocol_on_flush(protocol_t* protocol, client_t* client, char* output, size_t len);

#endif