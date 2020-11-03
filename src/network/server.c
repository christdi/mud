#include "mud/network/server.h"
#include "mud/network/client.h"
#include "mud/util/mudstring.h"
#include "mud/log.h"

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * Allocates and returns a new server_t structure
**/
server_t * create_server_t(void) {
  server_t * server = calloc(1, sizeof *server);

  server->fd = 0;
  server->port = 0;
  server->backlog = 10;

  return server;
}


/**
 * Frees a server_t structure, will also free clients belonging to this server
**/
void free_server_t(server_t * server) {
  assert(server);

  free(server);
}


/**
 * Attempts to listen on server defined by the server parameter.  The reuse address and
 * non-blocking flags are set automatically.
 *
 * Returns -1 on error on 0 on success.  
**/
int listen_on_server(server_t * server) {
  assert(server);
  assert(server->port != 0);

  struct addrinfo hints;
  struct addrinfo *server_info;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  char port_string[PORT_SIZE];

  if (int_to_string(server->port, port_string) != 0) {
    zlog_error(nc, "Failed to convert port from integer to string.");

    return -1;
  }

  int status = 0;

  if ((status = getaddrinfo(0, port_string, &hints, &server_info)) != 0) {
    zlog_error(nc, "%s", gai_strerror(status));

    return -1;
  }

  server->fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

  if (!server->fd) {
    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }

  int yes = 1;

  if (setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) != 0) {
    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }

  if (fcntl(server->fd, F_SETFL, O_NONBLOCK) != 0) {
    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }

  if (bind(server->fd, server_info->ai_addr, server_info->ai_addrlen) != 0) {
    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }

  if (listen(server->fd, server->backlog) != 0) {
    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }

  freeaddrinfo(server_info);

  zlog_info(nc, "Successfully bound to port [%d]", server->port);

  return 0;
}


/**
 * Attempts to accept on the server defined by the server_t parameter.  The client_t
 * parameter is populated with the connection details of the client.  The non
 * blocking flag is also set on the client.
 *
 * Returns -1 on error or 0 on success.
**/
int accept_on_server(server_t * server, client_t * client) {
  struct sockaddr_storage remote_address;
  socklen_t remote_address_size = sizeof remote_address;

  client->fd = accept(server->fd, (struct sockaddr *)&remote_address, &remote_address_size);

  if (!client->fd) {
    zlog_error(nc, "%s", strerror(errno));
    
    return -1;
  }

  if (fcntl(client->fd, F_SETFL, O_NONBLOCK) != 0) {
    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }  

  return 0;
}

/**
 * Attempts to close the server defined by the server_t parameter.
 *
 * Returns -1 on error on 0 on success.
**/
int close_server(server_t * server) {
  assert(server);
  
  zlog_info(nc, "Closing server on port [%d] with descriptor [%d]", server->port, server->fd);

  if (close(server->fd) != 0) {
    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }

  return 0;
}