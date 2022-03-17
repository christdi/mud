#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "mud/data/linked_list/linked_list.h"

/**
 * Function prototypes
 **/
void queue_enqueue(linked_list_t* queue, void* data);
void* queue_dequeue(linked_list_t* queue);

#endif
