#include "mud/network/network.h"
#include "mud/network/client.h"
#include "mud/network/server.h"
#include "mud/structure/list.h"
#include "mud/structure/iterator.h"
#include "mud/log/log.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlog.h>
#include <sys/select.h>

void free_client_list_t(list_t * list);

void add_fd_to_master_set(network_t * network, int fd);
void remove_fd_from_master_set(network_t * network, int fd);
int prune_clients(network_t * network);


/**
 * Allocates and initialises a new network_t struct.
 *
 * Returns the newly allocated network_t.
**/
network_t * create_network_t(void) {
  network_t * network = calloc(1, sizeof * network);

  FD_ZERO(&network->master_set);

  network->connection_callback = create_callback_t();
  network->disconnection_callback = create_callback_t();
  network->input_callback = create_callback_t();

  network->servers = create_list_t();
  network->clients = create_list_t();

  return network;
}


/**
 * Frees an allocted network_t and all substructures.
**/
void free_network_t(network_t * network) {
  assert(network);
  assert(network->servers);
  assert(network->clients);

  free_callback_t(network->connection_callback);
  free_callback_t(network->disconnection_callback);
  free_callback_t(network->input_callback);

  client_t * client = NULL;
  it_t it = list_begin(network->clients);

  while ((client = (client_t *) it_get(it)) != NULL)  {
    free_client_t(client);

    it = it_next(it);
  }

  free_list_t(network->clients);

  server_t * server = NULL;
  it = list_begin(network->servers);

  while ((server = (server_t *) it_get(it)) != NULL) {
    free_server_t(server);

    it = it_next(it);
  }

  free_list_t(network->servers);

  free(network);
}


/**
 * Creates a server and begins listening to it on a given port.  It's fd
 * is added to the master set which is copied by the polling thread prior
 * to calling select.
 *
 * Returns -1 if unable to listen or add fd to the fd set.
 *
 * Returns 0 on success.
**/
int start_game_server(network_t * network, unsigned int port) {
  assert(network);
  assert(port > 0);

  server_t * server = create_server_t();
  server->port = port;
  server->backlog = 10;

  if (listen_on_server(server) == -1) {
    zlog_error(nc, "Failed to listen on server on port [%d]", port);
    free_server_t(server);

    return -1;
  }

  add_fd_to_master_set(network, server->fd);
  list_add(network->servers, server);

  return 0;
}


/**
 * Sets a callback to be called when a client is accepted.
**/
void register_connection_callback(network_t * network, callback_func func, void * context) {
  assert(network);
  assert(func);
  assert(context);

  network->connection_callback->func = func;
  network->connection_callback->context = context;
}


/**
 * Sets a callback to be called when a client is pruned.
**/
void register_disconnection_callback(network_t * network, callback_func func, void * context) {
  assert(network);
  assert(func);
  assert(context);

  network->disconnection_callback->func = func;
  network->disconnection_callback->context = context;
}


/**
 * Sets a callback to be called has input.
**/
void register_input_callback(network_t * network, callback_func func, void * context) {
  assert(network);
  assert(func);
  assert(context);

  network->input_callback->func = func;
  network->input_callback->context = context;
}


/**
 * Internal method which contains the logic to poll the network for activity.
 * Uses select to determine if we have read activity on a server or client and
 * accepts or reads as appropriate.
**/
void poll_network(network_t * network) {
  assert(network);

  prune_clients(network); 

  fd_set read_set = network->master_set;

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;    

  int results = select(network->max_fd + 1, &read_set, NULL, NULL, &timeout);

  if (results == -1) {
    if (errno == EWOULDBLOCK) {
      return;
    }

    zlog_error(nc, "%s", strerror(errno));

    return;    
  }

  if (results == 0) {
    return;
  }

  if (results > 0) {
    server_t * server;
    it_t server_it = list_begin(network->servers);
      

    while ((server = (server_t *) it_get(server_it)) != NULL) {
      if (FD_ISSET(server->fd, &read_set)) {
        client_t * client = create_client_t();

        if (accept_on_server(server, client) != 0) {
          free_client_t(client);

          return;
        }

        add_fd_to_master_set(network, client->fd);
        list_add(network->clients, client);

        zlog_info(nc, "Client descriptor [%d] connected", client->fd);

        if (network->connection_callback->func) {
          network->connection_callback->func(client, network->connection_callback->context);
        }
      }

      server_it = it_next(server_it);
    }

    it_t client_it = list_begin(network->clients);
    client_t * client;

    while ((client = (client_t *) it_get(client_it)) != NULL) {
      if (FD_ISSET(client->fd, &read_set)) {
        if (receive_from_client(client) != 0) {
          zlog_error(nc, "Failed to read from client fd [%d]", client->fd);
        } else {
          if (network->input_callback->func) {
            network->input_callback->func(client, network->input_callback->context);
          }
        }
      }

      client_it = it_next(client_it);
    }      
  }

  return;
}


/**
 * Disconnect all clients currently connected to the network.  This closes the client,
 * removes the fd from the mster set, removes the client from the client list and then
 * frees the client.  This method does not call disconnection callbacks.  It is intended
 * to clear down the client list when shutting down.
**/
void disconnect_clients(network_t * network) {
  assert(network);

  client_t * client = NULL;

  it_t it = list_begin(network->clients);

  while ((client = (client_t *) it_get(it)) != NULL) {
    close_client(client);
    remove_fd_from_master_set(network, client->fd);
    it = list_remove(network->clients, client);
    free_client_t(client);
  }  
}


/**
 * Checks to see if clients have disconnected (have their hungup flag set to 1)
 * and closes them on our side, removes them from the master fd_set and from our
 * internal list of clients before releasing their memory.
 *
 * Returns 0 on success
**/
int prune_clients(network_t * network) {
  assert(network);
  assert(network->clients);

  client_t * client = NULL;

  it_t it = list_begin(network->clients);

  while ((client = (client_t *) it_get(it)) != NULL) {  
    if (client->hungup == 1) {
      zlog_info(nc, "Client descriptor [%d] disconnected", client->fd);

      if (close_client(client) == -1) {
        zlog_error(nc, "Failed to close hungup client");
      }

      remove_fd_from_master_set(network, client->fd);

      it = list_remove(network->clients, client);

      if (network->disconnection_callback->func) {
        network->disconnection_callback->func(client, network->disconnection_callback->context);
      }

      free_client_t(client);

    } else {
      it = it_next(it);
    }
  }

  return 0;
}


/**
 * Searches our internal list of servers and determines if we have one on a given
 * port, if we do, the server is closed, it's fd removed from the master fd_set, it
 * is removed from our internal server list and it's memory is freed
 *
 * Returns -1 if unable to stop a server or 0 on success.
**/
int stop_game_server(network_t * network, unsigned int port) {
  assert(network);

  it_t it = list_begin(network->servers);
  server_t * server;

  while ((server = (server_t *) it_get(it)) != NULL) {
    if (server->port == port) {
      if (close_server(server) == -1) {
        zlog_error(nc, "Failed to close server on port [%d]", port);
      }

      remove_fd_from_master_set(network, server->fd);
      list_remove(network->servers, server);
      free_server_t(server);

      return 0;
    }

    it = it_next(it);
  }

  return -1;
}


/**
 * Adds an fd to the master set and tracks the maximum fd.
 *
 * Returns -1 on failure or 0 on success.
**/
void add_fd_to_master_set(network_t * network, int fd) {
  FD_SET(fd, &network->master_set);

  if (fd > network->max_fd) {
    network->max_fd = fd;
  }
}


/**
 * Removes an FD from the master fd set
**/
void remove_fd_from_master_set(network_t * network, int fd) {
  FD_CLR(fd, &network->master_set);
}