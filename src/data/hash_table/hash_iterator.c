#include "mud/data/hash_table/hash_iterator.h"
#include "mud/data/hash_table/hash_node.h"
#include "mud/data/hash_table/hash_table.h"
#include "mud/data/linked_list/linked_list.h"
#include "mud/data/linked_list/node.h"
#include "mud/log.h"

/**
 * Retrieves the next hash node in a hash table.  Internally iterates over the
 * linked list that each node uses.
 **/
h_it_t h_it_next(h_it_t iter) {
  iter.list_iterator = it_next(iter.list_iterator);

  hash_node_t* node = (hash_node_t*)it_get(iter.list_iterator);

  if (node) {
    iter.node = node;

    return iter;
  }

  while (++iter.index < iter.capacity) {
    linked_list_t* list = iter.hash_table->nodes[iter.index];

    if (list != NULL) {
      iter.list_iterator = list_begin(list);
      iter.node = (hash_node_t*)it_get(iter.list_iterator);

      return iter;
    }
  }

  iter.node = NULL;

  return iter;
}

/**
 * Retrieves the value from the hash_node the h_it_t is currently looking at.
 **/
void* h_it_get(h_it_t iter) {
  if (iter.node) {
    return iter.node->value;
  }

  return NULL;
}

h_it_t hash_table_iterator(hash_table_t* table) {
  h_it_t iter;

  iter.hash_table = table;
  iter.capacity = HASH_TABLE_SIZE;
  iter.index = 0;

  for (iter.index = 0; iter.index < iter.capacity; iter.index++) {
    linked_list_t* list = table->nodes[iter.index];

    if (list != NULL) {
      iter.node = (hash_node_t*)list->first->data;

      iter.list_iterator = list_begin(table->nodes[iter.index]);

      return iter;
    }
  }

  iter.index = HASH_TABLE_SIZE;
  iter.node = NULL;

  return iter;
}
