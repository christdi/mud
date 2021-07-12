#include "mud/data/linked_list/node.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

node_t* node_new(void) {
  node_t* node = calloc(1, sizeof *node);

  node->data = NULL;
  node->next = NULL;
  node->prev = NULL;

  return node;
}

void node_free(node_t* node) {
  assert(node);

  if (node->deallocator) {
    node->deallocator(node->data);
  }

  free(node);
}
