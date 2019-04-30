#include "mud/structure/queue.h"

#include <assert.h>
#include <stdlib.h>

queue_t *queue_new() {
  queue_t *queue = calloc(1, sizeof *queue);

  queue->list = list_new();

  return queue;
}

int queue_push(queue_t *queue, node_t *node) {
  assert(queue);
  assert(node);

  return list_insert(queue->list, node);
}

int queue_pop(queue_t *queue, node_t **node) {
  assert(queue);

  list_last(queue->list, node);

  if (!(*node)) {
    return -1;
  }

  return list_remove(queue->list, *node, NULL);
}

int queue_clear(queue_t *queue) {
  assert(queue);

  return list_clear(queue->list);
}

int queue_is_empty(queue_t *queue) {
  assert(queue);

  return list_count(queue->list) > 0 ? 1 : 0;
}

void queue_free(queue_t *queue) {
  assert(queue);

  if (queue->list) {
    list_free(queue->list);

    queue->list = NULL;
  }

  free(queue);
  queue = NULL;
}
