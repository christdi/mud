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

  if (hash_node->deallocator != NULL) {
    hash_node->deallocator(hash_node->value);
  }

  free(hash_node);
}

/**
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
 **/
void deallocate_hash_node_t(void* value) {
  assert(value);

  hash_node_t* hash_node = (hash_node_t*)value;

  free_hash_node_t(hash_node);
}
