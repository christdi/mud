#include "mud/network/client.h"
#include "mud/mudstring.h"
#include "mud/structure/node.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <zlog.h>

client_t *network_client_new() {
  client_t *client = calloc(1, sizeof *client);

  client->fd = 0;
  client->thread = 0;
  client->hungup = 0;

  client->inputQueue = queue_new();

  return client;
}

int network_client_send(client_t *client, char *data) {
  assert(client);
  assert(data);

  zlog_category_t *networkCategory = zlog_get_category("network");

  unsigned long len = strlen(data);
  long bytes_sent = 0;

  while (bytes_sent < (long)len) {
    data = data + bytes_sent;

    bytes_sent = send(client->fd, data, len, 0);

    if (bytes_sent == -1L) {
      zlog_error(networkCategory, "%s", strerror(errno));

      return -1;
    } else if (bytes_sent == 0) {
      return -1;
    }
  }

  return 0;
}

int network_client_create_thread(client_t *client) {
  assert(client);

  zlog_category_t *networkCategory = zlog_get_category("network");

  if (pthread_create(&client->thread, NULL, network_client_receive_thread,
                     client) != 0) {
    zlog_error(networkCategory, "%s", strerror(errno));

    return -1;
  }

  zlog_debug(networkCategory, "Successfully created server accept thread.");

  return 0;
}

void *network_client_receive_thread(void *receiveThreadData) {
  assert(receiveThreadData);

  zlog_category_t *networkCategory = zlog_get_category("network");

  client_t *client = (client_t *)receiveThreadData;

  ssize_t len = 0;

  while (client->fd > 0) {
    char *buffer = calloc(1024, sizeof(char));

    if ((len = recv(client->fd, buffer, 1024, 0)) == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        continue;
      }

      zlog_error(networkCategory, "%s", strerror(errno));
      break;
    }

    if (len == 0) {
      client->hungup = 1;

      break;
    }

    buffer = string_remove(buffer, '\r');

    char *position;

    char *token = strtok_r(buffer, "\n", &position);

    if (token != NULL) {
      do {
        node_t *node = node_new();
        node->data = strdup(token);

        queue_push(client->inputQueue, node);
      } while ((token = strtok_r(NULL, "\n", &position)) != NULL);
    }

    free(buffer);
  }

  return 0;
}

int network_client_join_thread(client_t *client) {
  assert(client);
  assert(client->thread);

  zlog_category_t *networkCategory = zlog_get_category("network");

  if (pthread_join(client->thread, 0) != 0) {
    zlog_error(networkCategory, "%s", strerror(errno));

    return -1;
  }

  return 0;
}

int network_client_close(client_t *client) {
  assert(client);

  zlog_category_t *networkCategory = zlog_get_category("network");

  if (client->fd) {
    if (close(client->fd) != 0) {
      zlog_error(networkCategory, "%s", strerror(errno));

      return -1;
    }

    client->fd = 0;
  }

  return 0;
}

void network_client_free(client_t *client) {
  assert(client);

  if (client->inputQueue) {
    node_t *node = NULL;

    while (queue_pop(client->inputQueue, &node) != -1) {
      char *line = (char *)node->data;

      free(line);
    }

    queue_free(client->inputQueue);
    client->inputQueue = NULL;
  }

  free(client);
  client = NULL;
}
