#ifndef HG_EVENT_H
#define HG_EVENT_H


/**
 * Structs
**/
typedef struct event {
	void * data;
} event_t;


/**
 * Function prototypes
**/
event_t * create_event_t();
void free_event_t(event_t * event);


#endif