#include "mud/data/hash_table/hash_node.h"

#include <assert.h>
#include <stdlib.h>

/**
 * Allocates and initialises a new hash node.
 *
 * Returns the allocated hash node.
**/
hash_node_t* create_hash_node_t() {
  hash_node_t* hash_node = calloc(1, sizeof *hash_node);

  return hash_node;
}

/**
 * Frees a hash node.  This does not free the value it points to.
 * The user is expected to handle that.
**/
void free_hash_node_t(hash_node_t* hash_node) {
  assert(hash_node);
  assert(hash_node->key);

  free(hash_node->key);
  free(hash_node);

  hash_node = NULL;
}