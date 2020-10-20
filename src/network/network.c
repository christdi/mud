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

void * poll_network(void * parameter);
int add_fd_to_master_set(network_t * network, int fd);
int remove_fd_from_master_set(network_t * network, int fd);
int prune_clients(network_t * network);


/**
 * Allocates and initialises a new network_t struct.
 *
 * Returns the newly allocated network_t.
**/
network_t * create_network_t(void) {
  network_t * network = calloc(1, sizeof * network);

  network->thread = 0;
  pthread_mutex_init(&network->lock, NULL);
  network->shutdown = 0;

  FD_ZERO(&network->master_set);

  network->max_fd = 0;
  network->servers = list_new();
  network->clients = list_new();

  return network;
}


/**
 * Frees an allocted network_t and all substructures.
**/
void free_network_t(network_t * network) {
  assert(network);
  assert(network->servers);
  assert(network->clients);

  pthread_mutex_destroy(&network->lock);

  client_t * client = NULL;
  it_t it = list_begin(network->clients);

  while ((client = (client_t *) it_get(it)) != NULL)  {
    free_client_t(client);

    it = it_next(it);
  }

  list_free(network->clients);

  server_t * server = NULL;
  it = list_begin(network->servers);

  while ((server = (server_t *) it_get(it)) != NULL) {
    free_server_t(server);

    it = it_next(it);
  }

  free(network);
}


/**
 * Starts an internal thread which polls the network for new connections and
 * data available to read on clients.
 *
 * Returns -1 if unable to create the thread or 0 on success.
**/
int initialise_network(network_t * network) {
  if (pthread_create(&network->thread, NULL, poll_network, network) == -1) {
    zlog_error(nc, "Failed to initialise network polling thread");

    return -1;
  }

  return 0;
}


/**
 * Toggles the flag which will cause the polling thread to exit.  Uses a mutex
 * to ensure the operation is thread safe.
 *
 * Returns -1 if unable to lock or release the lock on the mutex or 0 on success.
**/
int shutdown_network(network_t * network) {
  if (pthread_mutex_lock(&network->lock) != 0) {
    zlog_error(nc, "Failed to lock network mutex");

    return -1;
  }

  network->shutdown = 1;


  if (pthread_mutex_unlock(&network->lock) != 0 ) {
    zlog_error(nc, "Failed to unlock network mutex");
    return -1;
  }

  return 0;
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

  if (add_fd_to_master_set(network, server->fd) == -1) {
    zlog_error(nc, "Failed to add server on port [%d] to fd set", port);
    close_server(server);
    free_server_t(server);

    return -1;
  };

  list_add(network->servers, server);

  return 0;
}


/**
 * Internal method which contains the logic to poll the network for activity.
 * Uses select to determine if we have read activity on a server or client and
 * accepts or reads as appropriate.
**/
void * poll_network(void * parameter) {
  assert(parameter);

  network_t * network = (network_t *) parameter;

  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  while (1) {

    if (pthread_mutex_lock(&network->lock) != 0) {
      zlog_error(nc, "Unable to obtain network lock, ending network thread");
      break;
    };

    if (network->shutdown) {
      if (pthread_mutex_unlock(&network->lock) != 0) {
        zlog_error(nc, "Unable to release network lock, ending network thread");
      } 

      break;
    }
  
    fd_set read_set = network->master_set;

    if (pthread_mutex_unlock(&network->lock) != 0) {
      zlog_error(nc, "Unable to release network lock, ending network thread");
      break;
    } 

    int results = select(network->max_fd + 1, &read_set, NULL, NULL, &timeout);

    if (results == -1) {
      if (errno == EWOULDBLOCK) {
        continue;
      }

      zlog_error(nc, "%s", strerror(errno));

      break;      
    }

    if (results == 0) {
      continue;
    }

    if (results > 0) {
      server_t * server;
      it_t server_it = list_begin(network->servers);
        

      while ((server = (server_t *) it_get(server_it)) != NULL) {
        if (FD_ISSET(server->fd, &read_set)) {
          client_t * client = create_client_t();

          if (accept_on_server(server, client) != 0) {
            free_client_t(client);

            continue;
          }

          if (add_fd_to_master_set(network, client->fd) != 0 ) {
            zlog_error(nc, "Failed to add client fd [%d] to fd set", client->fd);
            close_client(client);
            free_client_t(client);

            continue;            
          }

          list_add(network->clients, client);

          zlog_info(nc, "Client descriptor [%d] connected", client->fd);
          send_to_client(client, "Hello\n\r");        
        }

        server_it = it_next(server_it);
      }

      it_t client_it = list_begin(network->clients);
      client_t * client;

      while ((client = (client_t *) it_get(client_it)) != NULL) {
        if (FD_ISSET(client->fd, &read_set)) {
          if (receive_from_client(client) != 0) {
            zlog_error(nc, "Failed to read from client fd [%d]", client->fd);
          }
        }

        client_it = it_next(client_it);
      }      
    }

    prune_clients(network);    
  }

  return NULL;
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

      if (remove_fd_from_master_set(network, client->fd) != 0) {
        zlog_error(nc, "Unable to remove client fd [%d] from fd set", client->fd);
      } 

      it = list_remove(network->clients, client);

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

      if (remove_fd_from_master_set(network, server->fd) != 0) {
        zlog_error(nc, "Failed to remove server on port [%d] from fd set", port);
      }

      list_remove(network->servers, server);
      free_server_t(server);

      return 0;
    }

    it = it_next(it);
  }

  return -1;
}


/**
 * Thread safe method to add an fd to the master fd set.
 *
 * Returns -1 on failure or 0 on success.
**/
int add_fd_to_master_set(network_t * network, int fd) {
  if (pthread_mutex_lock(&network->lock) != 0) {
    zlog_error(nc, "Failed to lock network mutex");

    return -1;
  }

  FD_SET(fd, &network->master_set);

  if (fd > network->max_fd) {
    network->max_fd = fd;
  }

  if (pthread_mutex_unlock(&network->lock) != 0 ) {
    zlog_error(nc, "Failed to unlock network mutex");

    return -1;
  }    

  return 0;
}


/**
 * Thread safe method to remove an fd from the master fd set.
 *
 * Returns -1 on failure or 0 on success.
**/
int remove_fd_from_master_set(network_t * network, int fd) {
  if (pthread_mutex_lock(&network->lock) != 0) {
    zlog_error(nc, "Failed to lock network mutex");

    return -1;
  }

  FD_CLR(fd, &network->master_set);

  if (pthread_mutex_unlock(&network->lock) != 0 ) {
    zlog_error(nc, "Failed to unlock network mutex");

    return -1;
  }  

  return 0;
}