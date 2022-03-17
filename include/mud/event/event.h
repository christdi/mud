#ifndef MUD_EVENT_EVENT_H
#define MUD_EVENT_EVENT_H

#include <stdbool.h>

/**
 * Typedefs
 **/
typedef void (*event_deallocate_func_t)(void*);

typedef struct hash_table hash_table_t;
typedef struct linked_list linked_list_t;
typedef struct game game_t;

/**
 * Enum
 **/
typedef enum event_type {
  LUA_EVENT
} event_type_t;

/**
 * Structs
 **/
typedef struct event_broker {
  linked_list_t* events;
} event_broker_t;

typedef struct event {
  event_type_t type;
  void* data;
  event_deallocate_func_t deallocator;
} event_t;

/**
 * Function prototypes
 **/
event_t* event_new_event_t(event_type_t type, void* data, event_deallocate_func_t deallocator);
void event_free_event_t(event_t* event);

event_broker_t* event_new_event_broker_t();
void event_free_event_broker_t(event_broker_t* event_broker);

bool event_has_events(event_broker_t* event_broker);
void event_dispatch_events(event_broker_t* event_broker, game_t* game, hash_table_t* entities, hash_table_t* players);
void event_submit_event(event_broker_t* event_broker, event_t* event);

#endif