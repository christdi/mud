#include "mud/network/server.h"
#include "mud/log.h"

#include <assert.h>
#include <stdlib.h>

/**
 * Allocates and returns a new server_t structure
 **/
server_t* create_server_t(void) {
  server_t* server = calloc(1, sizeof *server);

  server->port = 0;
  server->backlog = 0;
  server->network = NULL;

  return server;
}

/**
 * Frees a server_t structure.
 **/
void free_server_t(server_t* server) {
  assert(server);

  free(server);
}
