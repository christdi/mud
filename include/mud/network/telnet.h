#ifndef MUD_NETWORK_TELNET_H
#define MUD_NETWORK_TELNET_H

/**
 * Typedefs
**/
typedef struct client client_t;


/**
 * Structs
**/
typedef enum telnet_option {
  NO, YES, WANT_NO, WANT_YES
} telnet_option_t;

typedef struct telnet {
  telnet_option_t echo;
  telnet_option_t suppress_go_ahead;
} telnet_t;

/**
 * Function prototypes
**/
telnet_t* network_new_telnet_t();
void network_free_telnet_t(telnet_t* telnet);
void network_deallocate_telnet_t(void* value);

void network_telnet_on_connection(client_t* client, void* protocol);
void network_telnet_on_input(client_t* client, void* protocol, char* input);
void network_telnet_on_output(client_t* client, void* protocol, char* output);

int network_telnet_send_will(client_t* client, char* option);
int network_telnet_send_wont(client_t* client, char* option);
int network_telnet_send_do(client_t* client, char* option);
int network_telnet_send_dont(client_t* client, char* option);


#endif