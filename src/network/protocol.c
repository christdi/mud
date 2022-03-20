#include <stdlib.h>
#include <assert.h>

#include "mud/network/client.h"
#include "mud/network/protocol.h"

/**
 * Allocates a new instance of protocol_t
 *
 * Returns the allocated instance
**/
protocol_t* network_new_protocol_t() {
  protocol_t* protocol = calloc(1, sizeof(protocol_t));

  return protocol;
}

/**
 * Frees an allocated instance of protocol_t
 *
 * protocol - the protocol_t instance to free
**/
void network_free_protocol_t(protocol_t* protocol) {
  assert(protocol);

  free(protocol);
}

/**
 * Walks the protocol chain and frees each of them.
 *
 * protocol - the first protocol_t instance in the chain to be freed
**/
void network_deallocate_protocol_chain(protocol_t* protocol) {
  while (protocol != NULL) {
    protocol_t* next = protocol->next;

    if (protocol->deallocator != NULL) {
      protocol->deallocator(protocol->data);
    }

    network_free_protocol_t(protocol);

    protocol = next;
  }
}

/**
 * Walks the protocol chain and calls the connected callback for each.
 *
 * client - the client_t instance whose protocol chain we're walking
**/
void network_protocol_chain_initialise(client_t* client) {
  protocol_t* protocol = client->protocol;

  while (protocol != NULL) {
    network_protocol_initialise(protocol, client);
    protocol = protocol->next;
  }
}

/**
 * Walks the protocol chain and calls the on input callback for each
 *
 * client - the client_t instance whose protocol chain we're walking
 * input - the input we've received
 * len - the amount of data received
 *
 * Returns the length of the input string as it may have been altered by protocols
**/
int network_protocol_chain_on_input(client_t* client, char* input, size_t len) {
  protocol_t* protocol = client->protocol;

  while (protocol != NULL) {
    len = network_protocol_on_input(protocol, client, input, len);
    protocol = protocol->next;
  }

  return len;
}

/**
 * Walks the protocol chain and calls the on output callback for each
 *
 * client - the client_t instance whose protocol chain we're walking
 * output - the output to be sent
 * len - the length of the output
 *
 * Returns the length of the output string as it may have been altered by protocols
**/
int network_protocol_chain_on_output(client_t* client, char* output, size_t len) {
  protocol_t* protocol = client->protocol;

  while(protocol != NULL) {
    len = network_protocol_on_output(protocol, client, output, len);
    protocol = protocol->next;
  }

  return len;
}

/**
 * Walks the protocol chain and calls the on flush callback for each
 *
 * client - the client_t instance whose protocol chain we're walking
 * output - the output that was flushed
 * len - the length of the output
**/
void network_protocol_chain_on_flush(client_t* client, char* output, size_t len) {
  protocol_t* protocol = client->protocol;

  while(protocol != NULL) {
    network_protocol_on_flush(protocol, client, output, len);
    protocol = protocol->next;
  }
}

/**
 * Calls the initialise callback of a protocol for a client
 *
 * protocol - protocol to call the initialise callback for
 * client - the client making use of the protocol
**/
void network_protocol_initialise(protocol_t* protocol, client_t* client) {
  assert(protocol);
  assert(client);

  if (protocol->initialiser != NULL) {
    protocol->initialiser(client, protocol->data);
  }
}

/**
 * Calls the on input callback of a protocol for a client
 *
 * protocol - protocol to call the on input callback for
 * client - the client making use of the protocol
 * input - the input received by the client
 * len - length of the input
 *
* Returns the length of the input string as it may have been altered by protocols
**/
int network_protocol_on_input(protocol_t* protocol, client_t* client, char* input, size_t len) {
  assert(protocol);
  assert(client);
  assert(input);

  if (protocol->on_input != NULL) {
    return protocol->on_input(client, protocol->data, input, len);
  }

  return len;
}

/**
 * Calls the on output callback of a protocol for a client
 *
 * protocol - protocol to call the on output callback for
 * client - the client making use of the protocol
 * output - the output to be sent to the client
 * len - length of the output
 *
* Returns the length of the output string as it may have been altered by protocols
**/
int network_protocol_on_output(protocol_t* protocol, client_t* client, char* output, size_t len) {
  assert(protocol);
  assert(client);
  assert(output);

  if (protocol->on_output != NULL) {
    return protocol->on_output(client, protocol->data, output, len);
  }

  return len;
}

/**
 * Calls the on flush callback of a protocol for a client
 *
 * protocol - protocol to call the on flush callback for
 * client - the client making use of the protocol
 * output -the output that was flushed
 * len - length of the output
**/
void network_protocol_on_flush(protocol_t* protocol, client_t* client, char* output, size_t len) {
  assert(protocol);
  assert(client);
  assert(output);

  if (protocol->on_flush != NULL) {
    protocol->on_flush(client, protocol->data, output, len);
  }
}
