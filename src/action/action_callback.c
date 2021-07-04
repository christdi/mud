#include <assert.h>
#include <stdlib.h>

#include "mud/action/action_callback.h"

/**
 * Allocate and iniitalise a new callback_t struct.
 *
 * Returns the allocated callback_struct.
**/
action_callback_t* create_action_callback_t() {
  action_callback_t* action_callback = calloc(1, sizeof *action_callback);

  action_callback->on_speak = NULL;

  return action_callback;
}

/**
 * Frees an action_callback_t struct.
**/
void free_action_callback_t(action_callback_t* action_callback) {
  assert(action_callback);

  free(action_callback);
}
