#ifndef MUD_EVENT_COMMUNICATE_H
#define MUD_EVENT_COMMUNICATE_H

/**
 * Typedefs
**/
typedef struct event event_t;
typedef struct entity entity_t;
typedef struct linked_list linked_list_t;

/**
 * Structs
**/
typedef struct communicate_event {
	entity_t* origin;
	linked_list_t* recipients;
	char* what;
} communicate_event_t;

/**
 * Function prototypes
**/
communicate_event_t* create_communicate_event_t();
void free_communicate_event_t(communicate_event_t* event);

event_t* communicate_event(entity_t *origin, linked_list_t* recipients, char *what);

#endif