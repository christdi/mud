#include "mud/structure/hash_table.h"
#include "mud/structure/hash_node.h"
#include "mud/log/log.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


int get_hash_index(char * key);


/**
 * Allocates and initialises a new hash_table struct.
 *
 * Returns the newly allocated hash_table.
**/
hash_table_t * create_hash_table_t() {
	hash_table_t * hash_table = calloc(1, sizeof * hash_table);

	return hash_table;
}


/**
 * Frees a hash_table
**/
void free_hash_table_t(hash_table_t * hash_table) {
	assert(hash_table);

	free(hash_table);

	hash_table = NULL;
}


/**
 * Gets a table index by generating a hash value and modulating it by MAX_TABLE_SIZE;
**/
int get_hash_index(char * key) {
	assert(key);

	size_t len = strnlen(key, MAX_KEY_LENGTH);

    unsigned long hash = 5381;
    int c;
    int i = 0;

    while ((c = *key++)) {
    	if (i++ == len) {
    		break;
    	} 

        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */    	
    }


    return hash % HASH_TABLE_SIZE;
}


/**
 * Inserts a value into a hash table with the supplied key.
 *
 * Returns 0 for success or -1 on failure.
**/
int hash_table_insert(hash_table_t * table, char * key, void * value) {
	assert(table);
	assert(key);
	assert(value);

	size_t len = strnlen(key, MAX_KEY_LENGTH - 1);

	if (len > MAX_KEY_LENGTH) {
		zlog_error(dc, "Hash key [%s] was too long and was truncated to [%d] characters", key, MAX_KEY_LENGTH);

		key[MAX_KEY_LENGTH] = '\0';
	}

	int index = get_hash_index(key);
	hash_node_t * hash_node = create_hash_node_t();
	hash_node->key = strdup(hash_node->key);
	hash_node->value = value;

	list_t * list = &table->nodes[index];
	list_add(list, value);

	return 0;
}


/**
 * Searches a hash table for a node with a given key and returns the value if found.
**/
void * hash_table_get(hash_table_t * table, char * key) {
	assert(table);
	assert(key);

	int index = get_hash_index(key);

	list_t * list = &table->nodes[index];
	it_t it = list_begin(list);
	hash_node_t * node;

	while ((node = (hash_node_t *) it_get(it)) != NULL) {
		if (strncmp(node->key, key, MAX_KEY_LENGTH) == 0) {
			return node->value;
		}

		it = it_next(it);
	}

	return NULL;
}


/**
 * Searches a hash table for a given key, deletes the node if found and returns the value
 * associated with the node so it can be deleted.
 *
 * Returns the value if found or NULL if no matching node is found.
**/
void * hash_table_delete(hash_table_t * table, char * key) {
	assert(table);
	assert(key);

	int index = get_hash_index(key);

	list_t * list = &table->nodes[index];
	it_t it = list_begin(list);
	hash_node_t * node;

	while ((node = (hash_node_t *) it_get(it)) != NULL) {
		if (strncmp(node->key, key, MAX_KEY_LENGTH) == 0) {
			list_remove(list, node);

			return node->value;
		}

		it = it_next(it);
	}

	return NULL;
}