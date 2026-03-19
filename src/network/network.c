#include "mud/network/network.h"
#include "mud/data/linked_list/iterator.h"
#include "mud/data/linked_list/linked_list.h"
#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/network/server.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uv.h>

static void on_new_connection(uv_stream_t* stream, int status);
static void on_client_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
static void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
static void on_client_close(uv_handle_t* handle);
static void on_client_close_silent(uv_handle_t* handle);
static void on_server_close(uv_handle_t* handle);

/**
 * Allocates and initialises a new network_t struct.
 *
 * Returns the newly allocated network_t.
 **/
network_t* create_network_t(void) {
  network_t* network = calloc(1, sizeof *network);

  network->loop = NULL;

  network->connection_callback = create_callback_t();
  network->disconnection_callback = create_callback_t();
  network->input_callback = create_callback_t();
  network->flush_callback = create_callback_t();

  network->servers = create_linked_list_t();
  network->clients = create_linked_list_t();

  return network;
}

/**
 * Frees an allocated network_t and all substructures.  By the time this is called
 * all client and server handles should have been closed and removed from their lists
 * via uv close callbacks during shutdown.
 **/
void free_network_t(network_t* network) {
  assert(network);
  assert(network->servers);
  assert(network->clients);

  free_callback_t(network->connection_callback);
  free_callback_t(network->disconnection_callback);
  free_callback_t(network->input_callback);
  free_callback_t(network->flush_callback);

  free_linked_list_t(network->clients);
  free_linked_list_t(network->servers);

  free(network);
}

/**
 * Creates a TCP server and begins listening on the given port.
 *
 * Returns -1 on failure or 0 on success.
 **/
int start_game_server(network_t* network, unsigned int port) {
  assert(network);
  assert(network->loop);
  assert(port > 0);

  server_t* server = create_server_t();
  server->port = port;
  server->backlog = BACKLOG;
  server->network = network;

  int res = uv_tcp_init(network->loop, &server->handle);

  if (res != 0) {
    LOG(ERROR, "uv_tcp_init failed: %s", uv_strerror(res));
    free_server_t(server);

    return -1;
  }

  server->handle.data = server;

  struct sockaddr_in6 addr;
  uv_ip6_addr("::", (int)port, &addr);

  res = uv_tcp_bind(&server->handle, (const struct sockaddr*)&addr, 0);

  if (res != 0) {
    LOG(ERROR, "uv_tcp_bind on port [%d] failed: %s", port, uv_strerror(res));
    uv_close((uv_handle_t*)&server->handle, NULL);
    free_server_t(server);

    return -1;
  }

  res = uv_listen((uv_stream_t*)&server->handle, (int)server->backlog, on_new_connection);

  if (res != 0) {
    LOG(ERROR, "uv_listen on port [%d] failed: %s", port, uv_strerror(res));
    uv_close((uv_handle_t*)&server->handle, NULL);
    free_server_t(server);

    return -1;
  }

  list_add(network->servers, server);

  LOG(INFO, "Successfully bound to port [%d]", port);

  return 0;
}

/**
 * Sets a callback to be called when a client is accepted.
 **/
void register_connection_callback(network_t* network, callback_func func, void* context) {
  assert(network);
  assert(func);
  assert(context);

  network->connection_callback->func = func;
  network->connection_callback->context = context;
}

/**
 * Sets a callback to be called when a client is pruned.
 **/
void register_disconnection_callback(network_t* network, callback_func func, void* context) {
  assert(network);
  assert(func);
  assert(context);

  network->disconnection_callback->func = func;
  network->disconnection_callback->context = context;
}

/**
 * Sets a callback to be called when a client has input.
 **/
void register_input_callback(network_t* network, callback_func func, void* context) {
  assert(network);
  assert(func);
  assert(context);

  network->input_callback->func = func;
  network->input_callback->context = context;
}

/**
 * Sets a callback to be called on client flush.
 *
 * network - instance of network_t.
 * func - callback function to be called on flush
 * context - void pointer to context that will be included in callback
 **/
void register_flush_callback(network_t* network, callback_func func, void* context) {
  assert(network);
  assert(func);
  assert(context);

  network->flush_callback->func = func;
  network->flush_callback->context = context;
}

/**
 * Checks all clients for pending output and flushes their buffers.
 *
 * network - network_t containing network context
 **/
void flush_output(network_t* network) {
  it_t it = list_begin(network->clients);
  client_t* client = NULL;

  while ((client = it_get(it)) != NULL) {
    if (client->output_length > 0) {
      if (network->flush_callback->func) {
        network->flush_callback->func(client, network->flush_callback->context);
      }

      flush_client_output(client);
    }

    it = it_next(it);
  }
}

/**
 * Closes all client handles without calling disconnection callbacks.
 * Used during engine shutdown to clear the client list.
 **/
void disconnect_clients(network_t* network) {
  assert(network);

  it_t it = list_begin(network->clients);
  client_t* client = NULL;

  while ((client = (client_t*)it_get(it)) != NULL) {
    it = it_next(it);

    if (!uv_is_closing((uv_handle_t*)&client->handle)) {
      uv_close((uv_handle_t*)&client->handle, on_client_close_silent);
    }
  }
}

/**
 * Initiates shutdown of all network handles (servers and clients).
 * Close callbacks remove items from lists and free memory.
 **/
void network_shutdown(network_t* network) {
  assert(network);

  it_t it = list_begin(network->clients);
  client_t* client = NULL;

  while ((client = (client_t*)it_get(it)) != NULL) {
    it = it_next(it);

    if (!uv_is_closing((uv_handle_t*)&client->handle)) {
      uv_close((uv_handle_t*)&client->handle, on_client_close_silent);
    }
  }

  it = list_begin(network->servers);
  server_t* server = NULL;

  while ((server = (server_t*)it_get(it)) != NULL) {
    it = it_next(it);

    if (!uv_is_closing((uv_handle_t*)&server->handle)) {
      uv_close((uv_handle_t*)&server->handle, on_server_close);
    }
  }
}

/**
 * Searches our internal list of servers for one on the given port and closes it.
 *
 * Returns -1 if server not found or 0 on success.
 **/
int stop_game_server(network_t* network, unsigned int port) {
  assert(network);

  it_t it = list_begin(network->servers);
  server_t* server = NULL;

  while ((server = (server_t*)it_get(it)) != NULL) {
    if (server->port == port) {
      if (!uv_is_closing((uv_handle_t*)&server->handle)) {
        uv_close((uv_handle_t*)&server->handle, on_server_close);
      }

      return 0;
    }

    it = it_next(it);
  }

  return -1;
}

/**
 * Called by libuv when a new TCP connection arrives on a listening server.
 **/
static void on_new_connection(uv_stream_t* stream, int status) {
  if (status < 0) {
    LOG(ERROR, "Connection error: %s", uv_strerror(status));

    return;
  }

  server_t* server = stream->data;
  network_t* network = server->network;

  client_t* client = create_client_t();
  client->network = network;

  int res = uv_tcp_init(network->loop, &client->handle);

  if (res != 0) {
    LOG(ERROR, "uv_tcp_init for client failed: %s", uv_strerror(res));
    free_client_t(client);

    return;
  }

  client->handle.data = client;

  if (uv_accept(stream, (uv_stream_t*)&client->handle) != 0) {
    uv_close((uv_handle_t*)&client->handle, NULL);
    free_client_t(client);

    return;
  }

  uv_os_fd_t ofd;
  uv_fileno((uv_handle_t*)&client->handle, &ofd);
  client->fd = (int)ofd;

  list_add(network->clients, client);

  LOG(INFO, "Client descriptor [%d] connected", client->fd);

  if (network->connection_callback->func) {
    network->connection_callback->func(client, network->connection_callback->context);
  }

  uv_read_start((uv_stream_t*)&client->handle, alloc_buffer, on_client_read);
}

/**
 * Called by libuv to request a buffer for incoming data.  Reads directly into
 * the client's input buffer at the current write position.
 **/
static void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  (void)suggested_size;
  client_t* client = handle->data;

  size_t existing = strnlen(client->input, CLIENT_BUFFER_LENGTH);
  size_t remaining = CLIENT_BUFFER_LENGTH - existing;

  if (remaining == 0) {
    buf->base = NULL;
    buf->len = 0;

    return;
  }

  buf->base = client->input + existing;
  buf->len = remaining;
}

/**
 * Called by libuv when data is available to read from a client connection.
 **/
static void on_client_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  client_t* client = stream->data;
  network_t* network = client->network;

  if (buf->base == NULL) {
    LOG(ERROR, "Disconnecting client fd [%d] as their input buffer is full.", client->fd);
    uv_close((uv_handle_t*)stream, on_client_close);

    return;
  }

  if (nread < 0) {
    if (nread != UV_EOF) {
      LOG(ERROR, "Read error on client fd [%d]: %s", client->fd, uv_strerror((int)nread));
    }

    uv_close((uv_handle_t*)stream, on_client_close);

    return;
  }

  if (nread == 0) {
    return;
  }

  char* data = buf->base;
  data[nread] = '\0';

  network_protocol_chain_on_input(client, data, (size_t)nread);

  client->last_active = time(NULL);

  if (network->input_callback->func) {
    network->input_callback->func(client, network->input_callback->context);
  }
}

/**
 * Called by libuv after a client handle is closed.  Notifies the application via
 * the disconnection callback then removes the client from the list and frees it.
 **/
static void on_client_close(uv_handle_t* handle) {
  client_t* client = handle->data;
  network_t* network = client->network;

  LOG(INFO, "Client descriptor [%d] disconnected", client->fd);

  list_remove(network->clients, client);

  if (network->disconnection_callback->func) {
    network->disconnection_callback->func(client, network->disconnection_callback->context);
  }

  free_client_t(client);
}

/**
 * Called by libuv after a client handle is closed during shutdown.  Removes the client
 * from the list and frees it without calling the disconnection callback.
 **/
static void on_client_close_silent(uv_handle_t* handle) {
  client_t* client = handle->data;
  network_t* network = client->network;

  list_remove(network->clients, client);
  free_client_t(client);
}

/**
 * Called by libuv after a server handle is closed.  Removes the server from
 * the list and frees it.
 **/
static void on_server_close(uv_handle_t* handle) {
  server_t* server = handle->data;
  network_t* network = server->network;

  list_remove(network->servers, server);
  free_server_t(server);
}
