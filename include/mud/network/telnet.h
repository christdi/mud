#ifndef MUD_NETWORK_TELNET_H
#define MUD_NETWORK_TELNET_H

#define TELNET_BUFFER_SIZE 128

/**
 * Typedefs
**/
typedef struct client client_t;
typedef struct protocol protocol_t;


/**
 * Structs
**/
typedef enum parse_state {
  READ_IAC, READ_OP, READ_OP_VALUE, DONE
} parse_state_t;

typedef enum telnet_option {
  NO, YES, WANT_NO, WANT_YES
} telnet_option_t;

typedef struct telnet {
  char buffer[TELNET_BUFFER_SIZE];
  parse_state_t state;
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

void network_telnet_initialised(client_t* client, void* protocol);
void network_telnet_on_input(client_t* client, void* protocol, char* input, size_t len);
void network_telnet_on_output(client_t* client, void* protocol, char* output, size_t len);

int network_telnet_send_ga(telnet_t* telnet, client_t* client);
int network_telnet_send_will(telnet_t* telnet, client_t* client, int option);
int network_telnet_send_wont(telnet_t* telnet, client_t* client, int option);
int network_telnet_send_do(telnet_t* telnet, client_t* client, int option);
int network_telnet_send_dont(telnet_t* telnet, client_t* client, int option);


#endif