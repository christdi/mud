#include "mud/list/list.h"
#include "mud/list/node.h"

#include <stdlib.h>

list_t * list_new() {
    list_t * list = calloc(1, sizeof * list);

    list->first = 0;
    list->last = 0;

}

void list_free(list_t * list) {
    if ( list ) {
        node_t * node = list->first;

        while ( node ) {
            node_t * next = node->next;

            list_node_free(node);

            node = next;
        }

        free(list);        
    }
}

void list_insert(list_t * list, node_t * node) {
    if ( !list->first ) {
        list->first = node;
        list->last = node;

        return;
    }

    node_t * last = list->last;

    last->next = node;
    node->prev = last;

    list->last = node;
}

void list_remove(list_t * list, node_t * node) {
    node_t * search = list->first;

    while ( search ) {
        node_t * current = search;

        if ( current == node ) {
            node_t * previous = current->prev;
            node_t * next = current->next;

            if ( current == list->first ) {
                list->first = next;
            }

            if ( current == list->last ) {
                list->last = previous;
            }

            if ( previous && next) {
                previous->next = next;
                next->prev = previous;
            }
        }

        search = current->next;
    }
}
