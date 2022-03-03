#include <assert.h>
#include <stdlib.h>

#include "lauxlib.h"

#include "mud/data/hash_table.h"
#include "mud/data/queue.h"
#include "mud/event/event.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/player.h"

/**
 * Allocates a new instance of an event_t.
 *
 * Parameters
 *   type - The type of the event
 *   data - Any data associated with the event
 *   deallocator - Function to deallocate data when event is freed
 *
 * Returns the allocated instance.
**/
event_t* event_new_event_t(event_type_t type, void* data, event_deallocate_func_t deallocator) {
  event_t* event = calloc(1, sizeof *event);

  event->type = type;
  event->data = data;
  event->deallocator = deallocator;

  return event;
}

/**
 * Frees an allocated instance of event_t.
 *
 * Parameters
 *   event - The event_t instance to be freed.
**/
void event_free_event_t(event_t* event) {
  assert(event);

  if (event->data != NULL) {
    if (event->deallocator != NULL) {
      event->deallocator(event->data);
    } else {
      LOG(WARN, "Potential memory leak.  Event type [%d] had data but no deallocator was specified.", event->type);
    }
  }

  free(event);
}

/**
 * Allocates a new instance of an event_broker.
 *
 * Returns the allocated instance
**/
event_broker_t* event_new_event_broker_t() {
  event_broker_t* event_broker = calloc(1, sizeof *event_broker);

  event_broker->events = create_linked_list_t();

  return event_broker;
}

/**
 * Frees an allocated instance of event_broker_t.
 *
 * Parameters
 *   event_broker - The event_broker_t instance to be freed
**/
void event_free_event_broker_t(event_broker_t* event_broker) {
  assert(event_broker);

  if (event_broker->events != NULL) {
    free_linked_list_t(event_broker->events);

    event_broker->events = NULL;
  }

  free(event_broker);
}

/**
 * Returns if the event_broker has any pending events.
 *
 * Parameters
 *   event_broker - The event_broker_t instance to check for events.
 *
 * Returns true if the broker has events, or false otherwise.
**/
bool event_has_events(event_broker_t* event_broker) {
  assert(event_broker);

  return list_size(event_broker->events) > 0;
}

/**
 * Dispatches any events in the event_broker_t to a given collection of entities and players.
 *
 * Parameters
 *   event_broker - The event_broker_t instance to retrieve events from.
 *   game - Instance of game_t containing pointers to data.
 *   entities - a linked list of entity_t types to submit events to.
 *   players - a linked list of player_t types to submit events to.
**/
void event_dispatch_events(event_broker_t* event_broker, game_t* game, hash_table_t* entities, hash_table_t* players) {
  assert(event_broker);
  assert(entities);
  assert(players);

  while (event_has_events(event_broker)) {
    event_t* event = queue_dequeue(event_broker->events);

    h_it_t it = hash_table_iterator(players);
    player_t* player = NULL;

    while ((player = h_it_get(it)) != NULL) {
      it = h_it_next(it);

      player_on_event(player, game, event);
    }

    event_free_event_t(event);
  }
}

/**
 * Submits a new event to be stored against the event_broker_t for the next dispatch.
 *
 * Parameters
 *   event_broker - the event_broker_t instance to store the event against.
 *   event - the event_t instance to be stored against the event broker.
**/
void event_submit_event(event_broker_t* event_broker, event_t* event) {
  assert(event_broker);
  assert(event_broker->events);

  queue_enqueue(event_broker->events, event);
}
