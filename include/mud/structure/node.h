#ifndef _NODE_H_
#define _NODE_H_

typedef struct node node_t;

struct node {
    void * data;
    node_t * next;
    node_t * prev;
};

node_t * list_node_new();
void list_node_free(node_t * node);

#endif
