#ifndef _CALLBACK_H_
#define _CALLBACK_H_

/**
 * Typedefs
**/
typedef struct client client_t;
typedef void (*callback_func)(client_t*, void*);

/**
 * Structs
**/
typedef struct callback {
  callback_func func;
  void* context;
} callback_t;

/**
 * Function prototypes
**/
callback_t* create_callback_t();
void free_callback_t(callback_t* callback);

#endif