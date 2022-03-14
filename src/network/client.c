#include "bsd/string.h"

#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/util/mudstring.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * Allocates memory for and initialises a new client_t struct.
 *
 * Returns the allocated client_t struct.
**/
client_t* create_client_t() {
  client_t* client = calloc(1, sizeof *client);

  client->fd = 0;
  client->hungup = 0;
  client->last_active = time(NULL);
  client->userdata = NULL;
  client->output_length = 0;

  return client;
}

/**
 * Frees a client_t struct.
**/
void free_client_t(client_t* client) {
  assert(client);

  free(client);

  client = NULL;
}

/**
 * Attempts to write output to the remote client represented by the client parameter.
 *
 * Returns 0 on success or -1 on failure.
**/
int send_to_client(client_t* client, char* data, size_t len) {
  assert(client);
  assert(data);

  if ((client->output_length + len) > CLIENT_BUFFER_LENGTH) {
    LOG(ERROR, "Send to client for fd [%d] would fail as data is too big to append to buffer", client->fd);

    return -1;
  }

  char* current = data;
  char* dest = client->output + client->output_length;
  size_t count = len;

  while(count > 0) {
    *dest++ = *current++;
    count--;
  }

  client->output_length += len;

  return 0;
}

/**
 * Flushes the contents of the output buffer and attempts to send it the remote endpoint.
 *
 * client - client_t instance whose output is being flushed
 * 
 * Returns 0 on success or -1 on error.
**/
int flush_output(client_t* client) {
  assert(client);
  
  if (client->output_length == 0) {
    return 0;
  }

  long bytes_sent = 0;

  char* data = client->output;

  while (bytes_sent < client->output_length) {
    data = data + bytes_sent;

    bytes_sent = send(client->fd, data, client->output_length, 0);

    if (bytes_sent == -1L) {
      LOG(ERROR, "%s", strerror(errno));

      return -1;
    }

    if (bytes_sent == 0) {
      return -1;
    }

    client->output_length = client->output_length - bytes_sent;
  }

  memset(client->output, 0, sizeof(client->output));

  return 0;
}

/**
 * Attempts to read data from the remote client.  If recv returns 0 or less, it is assumed the
 * client is no longer connected and it is mark as hungup.  Otherwise received data is passed
 * for appending to the client input buffer.
 *
 * Returns 0 on success or -1 on failure
**/
int receive_from_client(client_t* client) {
  assert(client);

  ssize_t len = 0;
  size_t existing = strnlen(client->input, CLIENT_BUFFER_LENGTH);

  if (existing == CLIENT_BUFFER_LENGTH) {
    LOG(ERROR, "Disconnecting client fd [%d] as their input buffer is full.", client->fd);
    client->hungup = 1;

    return -1;
  }

  size_t remaining = (CLIENT_BUFFER_SIZE)-existing;

  if ((len = recv(client->fd, client->input + existing, remaining - 1, 0)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }

    LOG(ERROR, "%s", strerror(errno));

    return -1;
  }

  if (len <= 0) {
    client->hungup = 1;
  } else {
    client->input[existing + len] = '\0';
    client->last_active = time(NULL);
  }

  return 0;
}

/**
 * Attempts to close the client.
 *
 * Returns 0 on success or -1 on failure.
**/
int close_client(client_t* client) {
  assert(client);

  if (client->fd) {
    if (close(client->fd) != 0) {
      LOG(ERROR, "%s", strerror(errno));

      return -1;
    }
  }

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

  return time(NULL) - client->last_active;
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
