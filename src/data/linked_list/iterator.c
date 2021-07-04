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
it_t it_next(it_t it) {
  it_t nextIt;

  if (it.node && it.node->next) {
    nextIt.node = it.node->next;
  } else {
    nextIt.node = NULL;
  }

  return nextIt;
}

/**
 * Given an iterator, creates a new iterator that represents the previous element
 * in the list.  If there is no previous element, the new iterator points to a
 * null value.
 *
 * Returns the new iterator
**/
it_t it_prev(it_t it) {
  it_t prevIt;

  if (it.node && it.node->prev) {
    prevIt.node = it.node->prev;
  } else {
    prevIt.node = NULL;
  }

  return prevIt;
}

/**
 * Given an iterator, attempts to retrieve the value stored by the node pointed to.
 *
 * Returns a void pointer to the data or NULL if no data is available.
**/
void* it_get(it_t it) {
  if (it.node && it.node->data) {
    return it.node->data;
  }

  return NULL;
}
