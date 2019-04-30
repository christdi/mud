#ifndef HG_QUEUE_H
#define HG_QUEUE_H

#include "mud/structure/list.h"

struct queue {
	list_t * list;
};

typedef struct queue queue_t;

queue_t * queue_new(void);
int queue_push(queue_t * queue, node_t * node);
int queue_pop(queue_t * queue, node_t ** node);
int queue_clear(queue_t * queue);
int queue_is_empty(queue_t * queue);
void queue_free(queue_t * queue);

#endif
