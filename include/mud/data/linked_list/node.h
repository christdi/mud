#ifndef _NODE_H_
#define _NODE_H_

/**
 * Typedefs
**/
typedef struct node node_t;
typedef void (*deallocate_func)(void*);

/**
 * Structs
**/
struct node {
  void* data;
  node_t* next;
  node_t* prev;
  deallocate_func deallocator;
};

/**
 * Function prototypes
**/
node_t* node_new(void);
void node_free(node_t* node);

#endif
