#include "mud/structure/queue.h"

#include <assert.h>
#include <stdlib.h>

/**
 * Syntactic sugar method to insert a node into a queue which is backed by a linked list.
**/
void queue_enqueue(list_t * queue, void * data) {
  assert(queue);
  assert(data);

  list_add(queue, data);
}

/**
 * Syntactic sugar method to retrieve and then remove the first entry from a queue backed 
 * by a linked list.
 *
 * Returns the data at the front of the linked list.
**/
void * queue_dequeue(list_t * queue) {
  assert(queue);

  it_t it = list_begin(queue);
  void * data = it_get(it);

  if (data) {
  	list_remove(queue, data);
  } 

  return data;
}
