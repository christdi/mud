#ifndef HG_HASH_NODE_H
#define HG_HASH_NODE_H


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
	char * key;
	void * value;

	hash_node_t * next;
};


/**
 * Function prototypes
**/
hash_node_t * create_hash_node_t();
void free_hash_node_t(hash_node_t * hash_node);


#endif