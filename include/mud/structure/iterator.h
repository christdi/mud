#ifndef HG_ITERATOR_H
#define HG_ITERATOR_H

#include "mud/structure/node.h"

/**
 * Structs
**/
typedef struct iterator {
	node_t * node;
} it_t;

/**
 * Function prototypes
**/
it_t it_next(it_t it);
it_t it_prev(it_t it);
void * it_get(it_t it);

#endif