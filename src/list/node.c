#include "mud/list/node.h"

#include <stdlib.h>

node_t * list_node_new() {
    node_t * node = calloc(1, sizeof * node);

    node->data = 0;
    node->next = 0;
    node->prev = 0;

	return node;
}

void list_node_free(node_t * node) {
    if ( node ) {
        free(node);
    }
}