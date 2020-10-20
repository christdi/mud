#include "mud/network/client.h"
#include "mud/mudstring.h"
#include "mud/log/log.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <zlog.h>

int append_data_to_input_buffer(client_t * client, char * data, size_t len);

/**
 * Allocates memory for and initialises a new client_t struct.
 *
 * Returns the allocated client_t struct.
**/
client_t * create_client_t() {
  client_t * client = calloc(1, sizeof *client);

  client->fd = 0;
  client->hungup = 0;

  return client;
}


/**
 * Frees a client_t struct.
**/
void free_client_t(client_t * client) {
  assert(client);
  
  free(client);

  client = NULL;
}


/**
 * Attempts to write output to the remote client represented by the client parameter.
 *
 * Returns 0 on success or -1 on failure.
**/
int send_to_client(client_t * client, char * data) {
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
    } else if (bytes_sent == 0) {
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
int receive_from_client(client_t * client) {
  assert(client);

  ssize_t len = 0;

  char bytes[MAX_RECV_SIZE] = {'\0'};

  if ((len = recv(client->fd, bytes, MAX_RECV_SIZE - 1, 0)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }

    zlog_error(nc, "%s", strerror(errno));

    return -1;
  }

  if (len <= 0) {
    client->hungup = 1;
  } else {
    if (append_data_to_input_buffer(client, bytes, len) != 0 ) {
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
int append_data_to_input_buffer(client_t * client, char * data, size_t len) {
  size_t existing = strlen(client->input);
  size_t total = existing + len + 1;

  if (total > MAX_INPUT_BUFFER_SIZE) {
    zlog_error(nc, "Client FD [%d] has filled their input buffer, disconnecting", client->fd);
    send_to_client(client, "Maximum input buffer was exceeded.  Disconnecting.\n\r");

    client->hungup = 1;

    return -1;
  }

  strcpy(client->input + existing, data);
  client->input[total] = '\0';

  return 0;
}


/**
 * Attempts to close the client.
 *
 * Returns 0 on success or -1 on failure.
**/
int close_client(client_t * client) {
  assert(client);

  if (client->fd) {
    if (close(client->fd) != 0) {
      zlog_error(nc, "%s", strerror(errno));

      return -1;
    }
  }

  return 0;
}
