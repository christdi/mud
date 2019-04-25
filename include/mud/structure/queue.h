#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "mud/structure/list.h"

struct queue {
	list_t * list;
};

typedef struct queue queue_t;

queue_t * queue_new();
const int queue_push(queue_t * queue, node_t * node);
const int queue_pop(queue_t * queue, node_t ** node);
const int queue_clear(queue_t * queue);
const int queue_is_empty(queue_t * queue);
void queue_free(queue_t * queue);

#endif