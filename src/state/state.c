#include <assert.h> /* assert */
#include <stdlib.h> /* calloc */

#include "mud/state/state.h"

/**
 * Allocates memory for a new instance of state_t.
 *
 * Returns the allocated state_t. 
**/
state_t* create_state_t() {
	state_t* state = calloc(1, sizeof *state);

	return state;
}

/**
 * Frees an alllocated state_t.
**/
void free_state_t(state_t* const state) {
	assert(state);

	free(state);
}