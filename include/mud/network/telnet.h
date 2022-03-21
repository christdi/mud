#ifndef MUD_NETWORK_TELNET_H
#define MUD_NETWORK_TELNET_H

#include <stdbool.h>

/**
 * Typedefs
**/
typedef struct telnet telnet_t;
typedef struct client client_t;
typedef struct protocol protocol_t;
typedef struct telnet_option telnet_option_t;
typedef struct telnet_config telnet_config_t;
typedef struct telnet_extension telnet_extension_t;

typedef void (*telnet_deallocator_func_t)(void*);
typedef void (*telnet_initialise_func_t)(void*, telnet_t*, client_t*);
typedef telnet_option_t* (*telnet_option_func_t)(void*, int);
typedef telnet_config_t* (*telnet_config_func_t)(void*, int);

/**
 * Structs
**/
typedef enum parse_state {
  READ_IAC, READ_OP, READ_OP_VALUE, READ_SE, DONE
} parse_state_t;

typedef enum option_state {
  NO, YES, WANT_NO, WANT_NO_OPPOSITE, WANT_YES, WANT_YES_OPPOSITE 
} option_state_t;

typedef struct telnet_option {
  option_state_t us;
  option_state_t them;
} telnet_option_t;

typedef struct telnet_parse {
  parse_state_t state;
  unsigned int op;
  unsigned int option;
} telnet_parse_t;

typedef struct telnet_config {
  int option;
  bool accept_will;
  bool accept_do;
} telnet_config_t;

typedef struct telnet_extension {
  void* extension;
  telnet_deallocator_func_t deallocate;
  telnet_initialise_func_t initialise;
  telnet_option_func_t get_option;
  telnet_config_func_t get_config;
  telnet_extension_t* next;
} telnet_extension_t;

typedef struct telnet {
  telnet_extension_t* extensions;
  telnet_parse_t incoming;
  telnet_parse_t outgoing; 
  telnet_option_t echo;
  telnet_option_t suppress_go_ahead;
} telnet_t;

/**
 * Function prototypes
**/
telnet_t* network_new_telnet_t();
void network_free_telnet_t(telnet_t* telnet);
void network_deallocate_telnet_t(void* value);

protocol_t* network_new_telnet_protocol_t();

void network_register_telnet_extension(telnet_t* telnet, telnet_extension_t* extension);

void network_telnet_initialised(client_t* client, void* protocol);
int network_telnet_on_input(client_t* client, void* protocol, char* input, size_t len);
int network_telnet_on_output(client_t* client, void* protocol, char* output, size_t len);
void network_telnet_on_flush(client_t* client, void* protocol, char* output, size_t len);

int network_telnet_send_ga(telnet_t* telnet, client_t* client);
int network_telnet_send_will(telnet_t* telnet, client_t* client, int option);
int network_telnet_send_wont(telnet_t* telnet, client_t* client, int option);
int network_telnet_send_do(telnet_t* telnet, client_t* client, int option);
int network_telnet_send_dont(telnet_t* telnet, client_t* client, int option);


#endif