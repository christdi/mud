#include "bsd/string.h"

#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/util/mudstring.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

typedef struct {
  uv_write_t req;
  char data[];
} write_req_t;

static void on_write_complete(uv_write_t* req, int status);

/**
 * Allocates memory for and initialises a new client_t struct.
 *
 * Returns the allocated client_t struct.
 **/
client_t* create_client_t(void) {
  client_t* client = calloc(1, sizeof *client);

  client->fd = 0;
  client->hungup = 0;
  client->last_active = time(NULL);
  client->userdata = NULL;
  client->protocol = NULL;
  client->network = NULL;
  client->output_length = 0;

  return client;
}

/**
 * Frees a client_t struct.
 **/
void free_client_t(client_t* client) {
  assert(client);

  if (client->protocol != NULL) {
    network_deallocate_protocol_chain(client->protocol);
  }

  free(client);
}

/**
 * Attempts to write output to the remote client represented by the client parameter.
 *
 * Returns 0 on success or -1 on failure.
 **/
int send_to_client(client_t* client, const char* data, size_t len) {
  assert(client);
  assert(data);

  if ((client->output_length + len) > CLIENT_BUFFER_LENGTH) {
    LOG(ERROR, "Send to client for fd [%d] would fail as data is too big to append to buffer", client->fd);

    return -1;
  }

  const char* current = data;
  char* dest = client->output + client->output_length;
  size_t count = len;

  while (count > 0) {
    *dest++ = *current++;
    count--;
  }

  client->output_length += len;

  return 0;
}

/**
 * Flushes the contents of the output buffer, runs it through the protocol chain and
 * submits it for async writing via libuv.
 *
 * client - client_t instance whose output is being flushed
 *
 * Returns 0 on success or -1 on error.
 **/
int flush_client_output(client_t* client) {
  assert(client);

  if (client->output_length == 0) {
    return 0;
  }

  network_protocol_chain_on_output(client, client->output, client->output_length);

  size_t len = client->output_length;

  write_req_t* wr = malloc(sizeof(write_req_t) + len);

  if (wr == NULL) {
    LOG(ERROR, "Failed to allocate write request for fd [%d]", client->fd);

    return -1;
  }

  memcpy(wr->data, client->output, len);

  uv_buf_t buf = uv_buf_init(wr->data, len);

  uv_write(&wr->req, (uv_stream_t*)&client->handle, &buf, 1, on_write_complete);

  network_protocol_chain_on_flush(client, client->output, len);

  memset(client->output, 0, sizeof(client->output));
  client->output_length = 0;

  return 0;
}

/**
 * Calculates how many seconds have passed since the client last sent data.
 *
 * Parameters
 *  - client - client_t structure whose data will be used to determine idle time;
 **/
int client_get_idle_seconds(const client_t* const client) {
  assert(client);

  return (int)(time(NULL) - client->last_active);
}

/**
 * Attempts to extract text from the input buffer.  The buffer is read character by character.
 * If the delim is encountered, we take all characters up to and including the delim and move
 * them into the character buffer referenced by dest and then append with a null character.
 *
 * Returns -1 if delim is not found.  Returns 0 if successful.
 **/
int extract_from_input(client_t* client, char* dest, size_t dest_len, const char* delim) {
  assert(client);
  assert(dest);

  size_t delim_len = strnlen(delim, DELIM_SIZE);

  size_t i = 0;
  size_t len = strnlen(client->input, CLIENT_BUFFER_SIZE);

  int ret = -1;

  for (i = 0; i < len; i++) {
    char* current = &client->input[i];

    if (strncmp(delim, current, delim_len) == 0) {
      if (i > dest_len) {
        LOG(ERROR, "Unable to extract input from client fd [%d], supplied dest buffer was too small at [%ld], needed [%ld]", client->fd, dest_len, i);
      } else {
        strncpy(dest, client->input, i);
        dest[i] = '\0';

        ret = 0;
      }

      memcpy(client->input, current + delim_len, len - i);

      break;
    }
  }

  return ret;
}

/**
 * Either sets an initial protocol or appends a protocol to the chain for the client.
 * If the client has a non-zero fd, it's assumed the protocol is being added to a
 * client that is already connected to an endpoint and the protocol should initialise
 * immediately.
 *
 * client - the client to set the protocol for
 * protocol - the protocol to set or append to the chain
 *
 * Returns 0 on success or -1 on failure
**/
int network_add_client_protocol(client_t* client, protocol_t* protocol) {
  assert(client);
  assert(protocol);

  if (client->protocol == NULL) {
    client->protocol = protocol;
  } else {
    protocol_t* chain = client->protocol;

    while (chain->next != NULL) {
      chain = protocol->next;
    }

    chain->next = protocol;
  }

  if (client->fd != 0) {
    network_protocol_initialise(protocol, client);
  }

  return 0;
}

/**
 * Determines if this client implements a protocol matching the type.
 *
 * client - the client whose protocols we are checking
 * protocol - an enum of the protocol type we're looking for
 *
 * Returns true if client has protocol or false otherwise
**/
bool network_client_has_protocol(client_t* client, protocol_type_t type) {
  protocol_t* protocol = client->protocol;

  while (protocol != NULL) {
    if (protocol->type == type) {
      return true;
    }

    protocol = protocol->next;
  }

  return false;
}

/**
 * Retrieves a protocol by type from a client
 *
 * client - the client for whom we are retrieving the protocol
 * protocol - an enum of the protocol we wish to retrieve
 *
 * Returns a pointer to the protocol or NULL if not found
**/
void* network_client_get_protocol(client_t* client, protocol_type_t type) {
  protocol_t* protocol = client->protocol;

  while (protocol != NULL) {
    if (protocol->type == type) {
      return protocol->data;
    }
  }

  return NULL;
}

/**
 * Called by libuv when an async write request completes.
 **/
static void on_write_complete(uv_write_t* req, int status) {
  write_req_t* wr = (write_req_t*)req;

  if (status < 0) {
    LOG(ERROR, "Write error: %s", uv_strerror(status));
  }

  free(wr);
}
