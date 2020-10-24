#include "mud/network/callback.h"

#include <stdlib.h>

/**
 * Allocate and initialise a new callback_t struct.
 *
 * Returns newly created callback_t struct.
**/
callback_t * create_callback_t() {
	callback_t * callback = calloc(1, sizeof * callback);

	return callback;
};


/**
 * Frees a callback_t instance.
**/
void free_callback_t(callback_t * callback) {
	free(callback);

	callback = NULL;
}