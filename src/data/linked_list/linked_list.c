#include "mud/data/linked_list/linked_list.h"
#include "mud/data/linked_list/node.h"
#include "mud/log.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void remove_node(linked_list_t* list, node_t* node);

/**
 * Allocates a new empty linked list.
 *
 * Returns the allocated list.
**/
linked_list_t* create_linked_list_t(void) {
  linked_list_t* list = calloc(1, sizeof *list);

  init_linked_list(list);

  return list;
}

void init_linked_list(linked_list_t* list) {
  list->first = NULL;
  list->last = NULL;

  pthread_mutex_init(&list->mutex, NULL);
}

/**
 * Frees a list.  The list must be empty before it can be successfully
 * freed as it doesn't know how to deallocate node data.  
**/
void free_linked_list_t(linked_list_t* list) {
  assert(list);

  if (list->first != NULL && list->last != NULL) {
    node_t* node = list->first;

    while (node != NULL) {
      remove_node(list, node);

      node = list->first;
    }
  }

  pthread_mutex_destroy(&list->mutex);

  free(list);
}

/**
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
**/
void deallocate_linked_list_t(void* value) {
  assert(value);

  linked_list_t* linked_list = (linked_list_t*)value;

  free_linked_list_t(linked_list);
}

/**
 * Add a node to the end of the linked list.  
**/
int list_add(linked_list_t* list, void* value) {
  assert(list);
  assert(value);

  if (pthread_mutex_lock(&list->mutex) != 0) {
    LOG(ERROR, "Failed to obtain mutex [%s]", strerror(errno));

    return -1;
  }

  node_t* node = node_new();
  node->data = value;
  node->deallocator = list->deallocator;

  if (!list->first) {
    list->first = node;
    list->last = node;
  } else {
    list->last->next = node;
    node->prev = list->last;
    node->next = NULL;
    list->last = node;
  }

  if (pthread_mutex_unlock(&list->mutex) != 0) {
    LOG(ERROR, "Failed to unlock mutex [%s]", strerror(errno));

    return -1;
  }

  return 0;
}

/**
 * Removes a node pointing to a given value from the linked list.
 *
 * Returns an iterator to the node after the one removed.
**/
it_t list_remove(linked_list_t* list, void* value) {
  assert(list);
  assert(value);

  it_t it;
  it.node = NULL;

  if (pthread_mutex_lock(&list->mutex) != 0) {
    LOG(ERROR, "Failed to obtain mutex [%s]", strerror(errno));

    return it;
  }

  node_t* node = list->first;

  while (node != NULL) {
    if (node->data == value) {
      it.node = node->next;
      remove_node(list, node);
      break;
    }

    node = node->next;
  }

  if (pthread_mutex_unlock(&list->mutex) != 0) {
    LOG(ERROR, "Failed to unlock mutex [%s]", strerror(errno));

    return it;
  }

  return it;
}

/**
 * Searches linked list src and removes values which return true for predicate and adds them
 * to dst.  Note that if a deallocator has been configured for the list that it will be explicitly
 * removed as the new linked list (or it's owner) is now responsible for management of the object.
 * 
 * Parameters
 *  src - the source linked list
 *  dst - the destination linked list
 *  predicate - the predicate which assesses the values 
**/
int list_extract(linked_list_t* src, linked_list_t* dst, linked_list_predicate_func_t predicate) {
  assert(src);
  assert(dst);
  assert(predicate);

  if (pthread_mutex_lock(&src->mutex) != 0) {
    LOG(ERROR, "Failed to obtain mutex [%s]", strerror(errno));

    return -1;
  }

  node_t* node = src->first;

  while (node != NULL) {
    node_t* next_node = node->next;
    void* value = node->data;

    if (predicate(value) > 0) {
      list_add(dst, value);

      node->deallocator = NULL;
      node->data = NULL;

      remove_node(src, node);
    }

    node = next_node;
  }

  if (pthread_mutex_unlock(&src->mutex) != 0) {
    LOG(ERROR, "Failed to unlock mutex [%s]", strerror(errno));

    return -1;
  }

  return 0;
}

/**
 * Attempts to retrieve the value stored at the supplied index node.
 *
 * Parameters
 *  list - the list the value is to be retrieved from
 *  index - the n index in the linked list to be retrieving
 *  value - output parameter which will be updated with a pointer to the value
 *
 * Returns 0 on success or -1 if not found.
**/
size_t list_at(linked_list_t* list, size_t index, void** value) {
  assert(list);

  if (pthread_mutex_lock(&list->mutex) != 0) {
    LOG(ERROR, "Failed to obtain mutex [%s]", strerror(errno));

    return -1;
  }

  size_t count = 0;
  size_t status = 0;
  node_t* node = list->first;

  while (node != NULL) {
    if (count == index) {
      *value = node->data;

      break;
    }

    count++;
  }

  if (node == NULL) {
    status = -1;
  }

  if (pthread_mutex_unlock(&list->mutex) != 0) {
    LOG(ERROR, "Failed to unlock mutex [%s]", strerror(errno));

    return -1;
  }

  return status;
}

/**
 * Unlinks and frees a node given the list and node.
**/
void remove_node(linked_list_t* list, node_t* node) {
  if (list->first == node) {
    list->first = node->next;
  }

  if (list->last == node) {
    list->last = node->prev;
  }

  if (node->prev != NULL) {
    node->prev->next = node->next;
  }

  if (node->next != NULL) {
    node->next->prev = node->prev;
  }

  node_free(node);
}

/**
 * Returns an iterator positioned at the start of the list.
**/
it_t list_begin(linked_list_t* list) {
  it_t it;

  it.node = list->first;

  return it;
}

/**
 * Returns an iterator positioned at the end of the list.
**/
it_t list_end(linked_list_t* list) {
  it_t it;

  it.node = list->last;

  return it;
}

/**
 * Calculates the amount of elements in the list.
 *
 * Returns the calculated amount of eleemnts in the list.
**/
int list_size(linked_list_t* list) {
  assert(list);

  if (pthread_mutex_lock(&list->mutex) != 0) {
    LOG(ERROR, "Failed to obtain mutex [%s]", strerror(errno));

    return 0;
  }

  int count = 0;

  node_t* node = NULL;

  for (node = list->first; node != NULL; node = node->next) {
    count++;
  }

  if (pthread_mutex_unlock(&list->mutex) != 0) {
    LOG(ERROR, "Failed to unlock mutex [%s]", strerror(errno));

    return 0;
  }

  return count;
}
