#include "mud/data/linked_list/linked_list.h"
#include "mud/data/linked_list/node.h"
#include "mud/log.h"

#include <assert.h>
#include <stdlib.h>


/**
 * Allocates a new empty linked list.
 *
 * Returns the allocated list.
**/
linked_list_t * create_linked_list_t(void) {
  linked_list_t * list = calloc(1, sizeof * list);

  list->first = NULL;
  list->last = NULL;

  pthread_mutex_init(&list->mutex, NULL);

  return list;
}


/**
 * Frees a list.  The list must be empty before it can be successfully
 * freed as it doesn't know how to deallocate node data.  
**/
void free_linked_list_t(linked_list_t * list) {
  assert(list);
  assert(list->first == NULL);
  assert(list->last == NULL);

  pthread_mutex_destroy(&list->mutex);

  free(list);
  list = NULL;
}


/**
 * Add a node to the end of the linked list.  
**/
void list_add(linked_list_t * list, void * value) {
  assert(list);
  assert(value);

  pthread_mutex_lock(&list->mutex);

  node_t * node = node_new();
  node->data = value;

  if (!list->first) {
    list->first = node;
    list->last = node;
  } else {
    list->last->next = node;
    node->prev = list->last;
    node->next = NULL;
    list->last = node;
  }
  
  pthread_mutex_unlock(&list->mutex);
}


/**
 * Removes a node pointing to a given value from the linked list.
 *
 * Returns an iterator to the node after the one removed.
**/
it_t list_remove(linked_list_t * list, void * value) {
  assert(list);
  assert(value);

  pthread_mutex_lock(&list->mutex);

  it_t it;
  it.node = NULL;

  node_t * node = list->first;

  for(node = list->first; node != NULL; node = node->next) {
    if (node->data == value) {
      it.node = node->next;

      if (list->first == node) {
        list->first = node->next;
      }

      if (list->last == node) {
        list->last = node->prev;
      }

      if (node->prev) {
        node->prev->next = node->next;
      }

      if (node->next) {
        node->next->prev = node->prev;
      }

      node_free(node);

      break;
    }
  }

  pthread_mutex_unlock(&list->mutex);

  return it;
}


/**
 * Returns an iterator positioned at the start of the list.
**/
it_t list_begin(linked_list_t * list) {
  it_t it;

  it.node = list->first;

  return it;
}


/**
 * Returns an iterator positioned at the end of the list.
**/
it_t list_end(linked_list_t * list) {
  it_t it;

  it.node = list->last;

  return it;
}


/**
 * Calculates the amount of elements in the list.
 *
 * Returns the calculated amount of eleemnts in the list.
**/
int list_size(linked_list_t * list) {
  assert(list);

  pthread_mutex_lock(&list->mutex);

  int count = 0;

  node_t * node;

  for (node = list->first; node != NULL; node = node->next) {
    count++;
  }

  pthread_mutex_unlock(&list->mutex);

  return count;
}
