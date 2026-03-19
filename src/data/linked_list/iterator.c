#include <assert.h>
#include <stdlib.h>

#include "mud/data/linked_list/iterator.h"
#include "mud/data/linked_list/node.h"

/**
 * Given an iterator, creates a new iterator that represents the next element
 * in the list.  If there is no next element, the new iterator points to a
 * null value.
 *
 * Returns the new iterator
 **/
it_t it_next(it_t iter) {
  it_t next_iter;

  if (iter.node && iter.node->next) {
    next_iter.node = iter.node->next;
  } else {
    next_iter.node = NULL;
  }

  return next_iter;
}

/**
 * Given an iterator, creates a new iterator that represents the previous element
 * in the list.  If there is no previous element, the new iterator points to a
 * null value.
 *
 * Returns the new iterator
 **/
it_t it_prev(it_t iter) {
  it_t prev_iter;

  if (iter.node && iter.node->prev) {
    prev_iter.node = iter.node->prev;
  } else {
    prev_iter.node = NULL;
  }

  return prev_iter;
}

/**
 * Given an iterator, attempts to retrieve the value stored by the node pointed to.
 *
 * Returns a void pointer to the data or NULL if no data is available.
 **/
void* it_get(it_t iter) {
  if (iter.node && iter.node->data) {
    return iter.node->data;
  }

  return NULL;
}
