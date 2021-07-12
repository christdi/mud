#ifndef _LIST_H_
#define _LIST_H_

#include "mud/data/linked_list/iterator.h"

#include <pthread.h>

/**
 * Typedefs
**/
typedef struct node node_t; /* linked_list/node.h */
typedef void (*deallocate_func)(void*);

/**
 * Structs
**/
typedef struct linked_list {
  pthread_mutex_t mutex;
  deallocate_func deallocator;
  node_t* first;
  node_t* last;
} linked_list_t;

/**
 * Function prototypes
**/
linked_list_t* create_linked_list_t(void);
void init_linked_list(linked_list_t* list);
void free_linked_list_t(linked_list_t* list);
void deallocate_linked_list_t(void* value);

void list_add(linked_list_t* list, void* value);
it_t list_remove(linked_list_t* list, void* value);
it_t list_begin(linked_list_t* list);
it_t list_end(linked_list_t* list);

int list_size(linked_list_t* list);

#endif