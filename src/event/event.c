#include <assert.h>
#include <stdlib.h>

#include "mud/event/event.h"

/**
 * Allocates and returns a pointer to a new event_t instance.
**/
event_t* create_event_t() {
  event_t* event = calloc(1, sizeof *event);

  event->type = UNDEFINED;
  event->data = NULL;

  return event;
}

/**
 * Frees an allocated event_t
**/
void free_event_t(event_t* event) {
  assert(event);

  free(event);
}
