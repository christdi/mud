#ifndef _HASH_NODE_H_
#define _HASH_NODE_H_

/**
 * Definitions
 **/
#define MAX_KEY_LENGTH 50

/**
 * Typedefs
 **/
typedef struct hash_node hash_node_t;
typedef void (*deallocate_func)(void*);

/**
 * Structs
 **/
struct hash_node {
  char* key;
  void* value;
  deallocate_func deallocator;
};

/**
 * Function prototypes
 **/
hash_node_t* create_hash_node_t();
void free_hash_node_t(hash_node_t* hash_node);
void deallocate_hash_node_t(void* value);

#endif