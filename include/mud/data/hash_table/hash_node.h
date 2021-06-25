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

/**
 * Structs
**/
struct hash_node {
  char* key;
  void* value;
};

/**
 * Function prototypes
**/
hash_node_t* create_hash_node_t();
void free_hash_node_t(hash_node_t* hash_node);

#endif