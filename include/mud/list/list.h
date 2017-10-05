#ifndef _LIST_H_
#define _LIST_H_

#include "node.h"

#include <pthread.h>

struct list {
	pthread_mutex_t mutex;
    node_t * first;
    node_t * last;
};

typedef struct list list_t;

list_t * list_new();
void list_free(list_t * list);

int list_insert(list_t * list, node_t * node);
int list_remove(list_t * list, node_t * node);
int list_next(list_t * list, node_t * node);
int list_prev(list_t * list, node_t * node);
int list_first(list_t * list, node_t * node);
int list_last(list_t * list, node_t * node);
int list_clear(list_t * list);

#endif
