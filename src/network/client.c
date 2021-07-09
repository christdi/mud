#include "bsd/string.h"

#include "mud/network/client.h"
#include "mud/log.h"
#include "mud/util/mudstring.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <zlog.h>

int append_data_to_input_buffer(client_t* client, char* data, size_t len);

/**
 * Allocates memory for and initialises a new client_t struct.
 *
 * Returns the allocated client_t struct.
**/
client_t* create_client_t() {
  client_t* client = calloc(1, sizeof *client);

  client->fd = 0;
  client->hungup = 0;

  generate_uuid(client->uuid, UUID_SIZE);

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
int send_to_client(client_t* client, char* data) {
  assert(client);
  assert(data);

  unsigned long len = strlen(data);
  long bytes_sent = 0;

  while (bytes_sent < (long)len) {
    data = data + bytes_sent;

    bytes_sent = send(client->fd, data, len, 0);

    if (bytes_sent == -1L) {
      zlog_error(nc, "%s", strerror(errno));

      return -1;
    }

    if (bytes_sent == 0) {
      return -1;
    }
  }

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

  char bytes[RECV_SIZE] = { '\0' };

  if ((len = recv(client->fd, bytes, RECV_SIZE - 1, 0)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }

    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }

  if (len <= 0) {
    client->hungup = 1;
  } else {
    bytes[len] = '\0';

    if (append_data_to_input_buffer(client, bytes, len) != 0) {
      zlog_error(nc, "Failed to append received data to input buffer");
    }
  }

  return 0;
}

/**
 * Appends data to the client input buffer.  First calculates if the received data
 * will fit in the input buffer.  If it does not, the client is marked as hungup so
 * it will be closed when next polled.
 *
 * Returns 0 on success or -1 on failure.
**/
int append_data_to_input_buffer(client_t* client, char* data, size_t len) {
  size_t existing = strnlen(client->input, INPUT_BUFFER_SIZE);
  size_t total = existing + len + 1;

  if (total > INPUT_BUFFER_SIZE) {
    zlog_error(nc, "Client FD [%d] has filled their input buffer, disconnecting", client->fd);
    send_to_client(client, "Maximum input buffer was exceeded.  Disconnecting.\n\r");

    client->hungup = 1;

    return -1;
  }

  strlcpy(client->input + existing, data, INPUT_BUFFER_SIZE - existing);

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
      zlog_error(nc, "%s", strerror(errno));

      return -1;
    }
  }

  return 0;
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
  size_t len = strnlen(client->input, INPUT_BUFFER_SIZE);

  int ret = -1;

  for (i = 0; i < len; i++) {
    char* current = &client->input[i];

    if (strncmp(delim, current, delim_len) == 0) {
      if (i > dest_len) {
        zlog_error(nc, "Unable to extract input from client [%s], supplied dest buffer was too small at [%ld], needed [%ld]", client->uuid, dest_len, i);
        send_to_client(client, "Your input was discarded as it was too long.\n\r");
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
