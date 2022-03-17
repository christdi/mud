#include <assert.h>
#include <stdlib.h>

#include "mud/narrator.h"

/**
 * Allocates a new instance of narrator_t.
 *
 * Parameters
 *   ref - Lua reference
 *
 * Returns an instance of narrator_t
 **/
narrator_t* narrator_new_narrator_t(int ref) {
  narrator_t* narrator = calloc(1, sizeof(narrator_t));

  return narrator;
}

/**
 * Frees an instance of narrator_t
 *
 * Parameters
 *   narrator - The instance of narrator_t to free
 **/
void narrator_free_narrator_t(narrator_t* narrator) {
  assert(narrator);

  free(narrator);
}
