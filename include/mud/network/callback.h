#ifndef HG_CALLBACK_H
#define HG_CALLBACK_H

#include "mud/network/client.h"

/**
 * Typedefs
**/
typedef void (*callback_func)(client_t *, void *);


/**
 * Structs
**/
typedef struct callback {
	callback_func func;
	void * context;
} callback_t;


/**
 * Function prototypes
**/
callback_t * create_callback_t();
void free_callback_t(callback_t * callback);

#endif