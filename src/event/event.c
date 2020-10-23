#include "mud/event/event.h"

#include <stdlib.h>


/**
 * Allocate and initialise a new event_t struct.
 *
 * Returns the allocated events
**/
event_t * create_event_t() {
	event_t * event = calloc(1, sizeof * event);

	return event;
}


/**
 * Frees an event_t struct.
**/
void free_event_t(event_t * event) {
	free(event);

	event = NULL;
}