#ifndef HG_LIST_H
#define HG_LIST_H

#include "mud/structure/node.h"

#include <pthread.h>

typedef struct list {
	pthread_mutex_t mutex;
    node_t * first;
    node_t * last;
} list_t;

list_t * list_new(void);
void list_free(list_t * list);

int list_insert(list_t * list, node_t * node);
int list_remove(list_t * list, node_t * node, node_t ** nextNode);
int list_clear(list_t * list);
int list_count(list_t * list);

void list_next(list_t * list, node_t ** node);
void list_prev(list_t * list, node_t ** node);
void list_first(list_t * list, node_t ** node);
void list_last(list_t * list, node_t ** node);

#endif
