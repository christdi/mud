#include "mud/structure/node.h"

#include <stdio.h>
#include <stdlib.h>

node_t * node_new() {
    node_t * node = calloc(1, sizeof * node);

    node->data = NULL;
    node->next = 0;
    node->prev = 0;

	return node;
}

void node_free(node_t * node) {
    if ( node ) {
        free(node);
        node = NULL;
    }
}