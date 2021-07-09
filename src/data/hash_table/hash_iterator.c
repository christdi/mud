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
h_it_t h_it_next(h_it_t it) {
  it.list_iterator = it_next(it.list_iterator);

  hash_node_t* node = (hash_node_t*)it_get(it.list_iterator);

  if (node) {
    it.node = node;

    return it;
  }

  while (++it.index < it.capacity) {
    linked_list_t* list = it.hash_table->nodes[it.index];

    if (list != NULL) {
      it.list_iterator = list_begin(list);
      it.node = (hash_node_t*)it_get(it.list_iterator);

      return it;
    }
  }

  it.node = NULL;

  return it;
}

/**
 * Retrieves the value from the hash_node the h_it_t is currently looking at.
**/
void* h_it_get(h_it_t it) {
  if (it.node) {
    return it.node->value;
  }

  return NULL;
}

h_it_t hash_table_iterator(hash_table_t* table) {
  h_it_t it;

  it.hash_table = table;
  it.capacity = HASH_TABLE_SIZE;
  it.index = 0;

  for (it.index = 0; it.index < it.capacity; it.index++) {
    linked_list_t* list = table->nodes[it.index];

    if (list != NULL) {
      it.node = (hash_node_t*)list->first->data;

      it.list_iterator = list_begin(table->nodes[it.index]);

      return it;
    }
  }

  it.index = HASH_TABLE_SIZE;
  it.node = NULL;
  it.list_iterator = list_begin(table->nodes[HASH_TABLE_SIZE]);

  return it;
}
