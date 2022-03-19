#include <stdlib.h>
#include <assert.h>

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
