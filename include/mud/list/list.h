#ifndef _LIST_H_
#define _LIST_H_

#include "node.h"

struct list {
    node_t * first;
    node_t * last;
};

typedef struct list list_t;

list_t * list_new();
void list_free(list_t * list);

void list_insert(list_t * list, node_t * node);
void list_remove(list_t * list, node_t * node);

#endif
