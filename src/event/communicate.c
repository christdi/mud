#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "mud/event/event.h"
#include "mud/event/communicate.h"

/**
 * Allocates and returns a pointer to a new communicate_event_t
**/
communicate_event_t* create_communicate_event_t() {
	communicate_event_t* event = calloc(1, sizeof *event);

	event->recipients = NULL;
	event->what = NULL;

	return event;
}

/**
 * Frees an allocated communicate_event_t.  This does not free the origin entity or recipient 
 * entites as the event does not own them.
**/
void free_communicate_event_t(communicate_event_t* event) {
	assert(event);

	if (event->what != NULL) {
		free(event->what);
	}

	free(event);
}

/**
 * Creates and returns a new communication event.
**/
event_t* communicate_event(entity_id_t origin, linked_list_t* recipients, char *what) {
	event_t* event = create_event_t();
	event->type = COMMUNICATION;

	communicate_event_t* communicate_event = create_communicate_event_t();
	communicate_event->origin = origin;
	communicate_event->recipients = recipients;
	communicate_event->what = strdup(what);

	event->data = (communicate_event_t*) communicate_event;

	return event;
}
