#ifndef HG_EVENT_H
#define HG_EVENT_H


/**
 * Enumerations
**/
typedef enum event_type {
	PLAYER_CONNECTED, PLAYER_DISCONNECTED, PLAYER_INPUT
} event_type_t;


/**
 * Structs
**/
typedef struct event {
	event_type_t type;
	void * data;
} event_t;


/**
 * Function prototypes
**/
event_t * create_event_t();
void free_event_t(event_t * event);


#endif