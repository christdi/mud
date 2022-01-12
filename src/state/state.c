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
void free_state_t(state_t* state) {
  assert(state);

  if (state->name != NULL) {
    free(state->name);
  }

  if (state->on_enter != NULL) {
    free(state->on_enter);
  }

  if (state->on_exit != NULL) {
    free(state->on_exit);
  }

  if (state->on_input != NULL) {
    free(state->on_input);
  }

  if (state->on_tick != NULL) {
    free(state->on_tick);
  }

  free(state);
}
