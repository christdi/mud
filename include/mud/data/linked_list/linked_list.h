#ifndef _LIST_H_
#define _LIST_H_

#include "mud/data/linked_list/node.h"
#include "mud/data/linked_list/iterator.h"

#include <pthread.h>

/**
 * Structs
**/
typedef struct linked_list {
	pthread_mutex_t mutex;
    node_t * first;
    node_t * last;
} linked_list_t;


/**
 * Function prototypes
**/
linked_list_t * create_linked_list_t(void);
void free_linked_list_t(linked_list_t * list);

void list_add(linked_list_t * list, void * value);
it_t list_remove(linked_list_t * list, void * value);
it_t list_begin(linked_list_t * list);
it_t list_end(linked_list_t * list);

int list_size(linked_list_t * list);

#endif
