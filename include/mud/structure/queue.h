#ifndef HG_QUEUE_H
#define HG_QUEUE_H

#include "mud/structure/list.h"

void queue_enqueue(list_t * queue, void * data);
void * queue_dequeue(list_t * queue);

#endif
