#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "mud/structure/list.h"

void queue_enqueue(list_t * queue, void * data);
void * queue_dequeue(list_t * queue);

#endif
