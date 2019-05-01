#ifndef HG_LIST_H
#define HG_LIST_H

#include "mud/structure/node.h"
#include "mud/structure/iterator.h"

#include <pthread.h>

/**
 * Structs
**/
typedef struct list {
	pthread_mutex_t mutex;
    node_t * first;
    node_t * last;
} list_t;


/**
 * Function prototypes
**/
list_t * list_new(void);
void list_free(list_t * list);

void list_add(list_t * list, void * value);
it_t list_remove(list_t * list, void * value);
it_t list_begin(list_t * list);
it_t list_end(list_t * list);

int list_size(list_t * list);

#endif
