#ifndef MUD_EVENT_EVENT_H
#define MUD_EVENT_EVENT_H

/**
 * Enums
**/
typedef enum event_type {
	UNDEFINED,
	COMMUNICATION
} event_type_t;

/**
 * Structs
**/
typedef struct event {
	event_type_t type;
	void* data;
} event_t;

/**
 * Function prototypes
**/
event_t* create_event_t();
void free_event_t(event_t* event);

#endif