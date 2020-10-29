#ifndef _HASH_ITERATOR_H_
#define _HASH_ITERATOR_H_


#include "mud/data/hash_table/hash_table.h"
#include "mud/data/hash_table/hash_node.h"
#include "mud/data/linked_list/iterator.h"

/**
 * Structs
**/
typedef struct hash_iterator {
	hash_table_t * hash_table;
	hash_node_t * node;

	size_t capacity;
	size_t index;

	it_t list_iterator;
} h_it_t;


/**
 * Function prototypes
**/
h_it_t h_it_next(h_it_t it);
void * h_it_get(h_it_t it);

h_it_t hash_table_iterator(hash_table_t * table);


#endif