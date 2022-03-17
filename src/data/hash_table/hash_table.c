#include "mud/data/hash_table/hash_table.h"
#include "mud/data/hash_table/hash_node.h"
#include "mud/log.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

unsigned int get_hash_index(const char* key);

/**
 * Allocates and initialises a new hash_table struct.
 *
 * Returns the newly allocated hash_table.
 **/
hash_table_t* create_hash_table_t() {
  hash_table_t* hash_table = calloc(1, sizeof *hash_table);

  return hash_table;
}

/**
 * Frees a hash_table
 **/
void free_hash_table_t(hash_table_t* hash_table) {
  assert(hash_table);

  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    linked_list_t* list = hash_table->nodes[i];

    if (list != NULL) {
      free_linked_list_t(list);
    }
  }

  free(hash_table);
}

/**
 * Gets a table index by generating a hash value and modulating it by MAX_TABLE_SIZE;
 **/
unsigned int get_hash_index(const char* key) {
  assert(key);

  size_t len = strnlen(key, MAX_KEY_LENGTH);

  unsigned long hash = HASH_BASE_VALUE;
  unsigned char c = 0;
  size_t i = 0;

  while ((c = (unsigned char)*key++)) {
    if (i++ == len) {
      break;
    }

    hash = ((hash << FIVE_BITS) + hash) + c; /* hash * 33 + c */
  }

  return (unsigned int)hash % HASH_TABLE_SIZE;
}

/**
 * Inserts a value into a hash table with the supplied key.
 *
 * Returns 0 for success or -1 on failure.
 **/
int hash_table_insert(hash_table_t* table, const char* key, void* value) {
  assert(table);
  assert(key);
  assert(value);

  char* hash_key = strdup(key);
  size_t len = strnlen(hash_key, MAX_KEY_LENGTH - 1);

  if (len > MAX_KEY_LENGTH) {
    LOG(ERROR, "Hash key [%s] was too long and was truncated to [%d] characters", key, MAX_KEY_LENGTH);

    hash_key[MAX_KEY_LENGTH] = '\0';
  }

  int index = get_hash_index(key);
  hash_node_t* hash_node = create_hash_node_t();
  hash_node->key = hash_key;
  hash_node->value = value;
  hash_node->deallocator = table->deallocator;

  if (table->nodes[index] == NULL) {
    linked_list_t* linked_list = create_linked_list_t();
    linked_list->deallocator = deallocate_hash_node_t;
    table->nodes[index] = linked_list;
  }

  linked_list_t* list = table->nodes[index];
  list_add(list, hash_node);

  return 0;
}

/**
 * Searches a hash table for a given key and deletes if found.
 *
 * Note that if a deallocator has not been set for the hash_table then the
 * value pointed to by the node is not freed.  It's the responsibility of
 * the caller to configure a deallocator or arrange for the value to be
 * freed first if relevant.
 **/
void hash_table_delete(hash_table_t* table, const char* key) {
  assert(table);
  assert(key);

  int index = get_hash_index(key);

  linked_list_t* list = table->nodes[index];

  if (list == NULL) {
    return;
  }

  it_t it = list_begin(list);
  hash_node_t* node = NULL;

  while ((node = (hash_node_t*)it_get(it)) != NULL) {
    if (strncmp(node->key, key, MAX_KEY_LENGTH) == 0) {
      list_remove(list, node);

      if (list_size(list) == 0) {
        free_linked_list_t(list);
        table->nodes[index] = NULL;
      }

      return;
    }

    it = it_next(it);
  }
}

/**
 * Determines if a key exists in a hash table.
 *
 * Returns 1 if the key exists, or 0 if not.
 **/
int hash_table_has(hash_table_t* table, const char* key) {
  assert(table);
  assert(key);

  int index = get_hash_index(key);

  linked_list_t* list = table->nodes[index];

  if (list == NULL) {
    return 0;
  }

  it_t it = list_begin(list);
  hash_node_t* node = NULL;

  while ((node = (hash_node_t*)it_get(it)) != NULL) {
    if (strncmp(node->key, key, MAX_KEY_LENGTH) == 0) {
      return 1;
    }

    it = it_next(it);
  }

  return 0;
}

/**
 * Searches a hash table for a node with a given key and returns the value if found.
 **/
void* hash_table_get(hash_table_t* table, const char* key) {
  assert(table);
  assert(key);

  int index = get_hash_index(key);

  linked_list_t* list = table->nodes[index];

  if (list == NULL) {
    return NULL;
  }

  it_t it = list_begin(list);
  hash_node_t* node = NULL;

  while ((node = (hash_node_t*)it_get(it)) != NULL) {
    if (strncmp(node->key, key, MAX_KEY_LENGTH) == 0) {
      return node->value;
    }

    it = it_next(it);
  }

  return NULL;
}
