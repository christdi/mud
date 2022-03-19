#include <stdlib.h>
#include <assert.h>

#include "mud/network/telnet.h"

/**
 * Allocates a new instance of telnet_t
 *
 * Returns the allocated instance
**/
telnet_t* network_new_telnet_t() {
  telnet_t* telnet = calloc(1, sizeof(telnet_t));

  return telnet;
}

/**
 * Frees an allocated instance of telnet_t
 *
 * telnet - the telnet_t instance to be freed
**/
void network_free_telnet_t(telnet_t* telnet) {
  assert(telnet);

  free(telnet);
}

/**
 * Deallocates a void pointer to telnet_t
 *
 * value - void pointer to an instance of telnet_t
**/
void network_deallocate_telnet_t(void* value) {
  assert(value);
  
  network_free_telnet_t(value);
}

/**
 * Callback method called when the client has established a connection.
 *
 * client - the client who has established a connection
 * protocol - a void pointer to a telnet_t instance
**/
void network_telnet_on_connection(client_t* client, void* protocol) {
  assert(client);
  assert(protocol);
}

/**
 * Callback method called when the client has received new input.
 *
 * client - the client who has received input
 * protocol - a void pointer to a telnet_t instance
 * data - the data that has been received
**/
void network_telnet_on_input(client_t* client, void* protocol, char* input) {
  assert(client);
  assert(protocol);
  assert(input);
}

/**
 * Callback method called when the client has received new output.
 *
 * client - the client who has received output
 * protocol - a void pointer to a telnet_t instance
 * data - the data that has been buffered for output
**/
void network_telnet_on_output(client_t* client, void* protocol, char* output) {
  assert(client);
  assert(protocol);
  assert(output);
}

/**
 * Sends an IAC WILL OPTION to the client.
 *
 * client - the client to sent the WILL to
 * option - the option to send WILL for
 *
 * Returns 0 on success or -1 on failure
**/
int network_telnet_send_will(client_t* client, char* option) {
  assert(client);
  assert(option);

  return 0;
}

/**
 * Sends an IAC WONT OPTION to the client.
 *
 * client - the client to sent the WONT to
 * option - the option to send WONT for
 *
 * Returns 0 on success or -1 on failure
**/
int network_telnet_send_wont(client_t* client, char* option) {
  assert(client);
  assert(option);

  return 0;
}

/**
 * Sends an IAC DO OPTION to the client.
 *
 * client - the client to sent the DO to
 * option - the option to send DO for
 *
 * Returns 0 on success or -1 on failure
**/
int network_telnet_send_do(client_t* client, char* option) {
  assert(client);
  assert(option);

  return 0;
}

/**
 * Sends an IAC DONT OPTION to the client.
 *
 * client - the client to sent the DONT to
 * option - the option to send DONT for
 *
 * Returns 0 on success or -1 on failure
**/
int network_telnet_send_dont(client_t* client, char* option) {
  assert(client);
  assert(option);

  return 0;
}
