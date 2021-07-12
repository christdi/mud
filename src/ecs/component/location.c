#include <assert.h>
#include <stdlib.h>

#include "mud/data/hash_table.h"
#include "mud/ecs/component/location.h"
#include "mud/ecs/components.h"
#include "mud/log.h"

/**
 * Allocates and initialises a new location_t struct.
 *
 * Returns a pointer to the newly allocated location_t;
**/
location_t* create_location_t() {
  location_t* location = calloc(1, sizeof *location);

  return location;
}

/**
 * Frees an allocated location_t struct.
**/
void free_location_t(location_t* location) {
  assert(location);

  free(location);
}

/**
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
**/
void deallocate_location_t(void* value) {
  assert(value);

  location_t* location = (location_t*)value;

  free_location_t(location);
}

/**
 * Checks if a given entity has a location component registered with game components..
 *
 * Accepts the following parameters;
 *   components - a pointer to a components struct containing component registration
 *   entity - a pointer to an entity struct representing the entity being checked
 *
 * Returns 1 if a location component is found for the entity, 0 if not.
**/
int has_location(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  return hash_table_has(components->location, entity->id.uuid);
}

/**
 * Registers a location with the game components.
 *
 * Accepts the following parameters:
 *   components - a pointer to a components struct representative of the game components
 *   location - a pointer to a location struct to be registered with game components
**/
void register_location(components_t* components, location_t* location) {
  assert(components);
  assert(location);

  if (hash_table_insert(components->location, location->entity_id.uuid, location) != 0) {
    zlog_error(gc, "Failed to register location component for entity uuid [%s]", location->entity_id.uuid);
  }
}

/**
 * Unregisters a location with the game components.
 *
 * Accepts the following parameters:
 *  components - a pointer to a components struct representative of the game components
 *  entity - a pointer to an entity struct representing the entity whose location
 *    component should be unregistered.
**/
void unregister_location(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  hash_table_delete(components->location, entity->id.uuid);
}

/**
 * Retrieves a location component from the game components.
 *
 * Accepts the following parameters:
 *  components - a pointer to a components struct representing the game components.
 *  entity - a pointer to an entity struct representing entity whose location component 
 *    is desired
**/
location_t* get_location(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  return (location_t*)hash_table_get(components->location, entity->id.uuid);
}

/**
 * Prints a description of a location component into the buffer pointed to by dest.
 *
 * Accepts the following parameters:
 *   location - a pointer to the location struct to be described
 *   dest - the destination string buffer for the description to be stored.  This
 *     must be allocated by the caller.
 *   len - The size of the destination string buffer.
**/
void describe_location(location_t* location, char* dest, size_t len) {
  snprintf(dest, len, "at = %s", location->at.uuid);
}

/**
 * Update all location components.
 *
 * ccepts the following parameters:
 *  game - A pointer to a game struct containing the components structure to be updated
**/
void update_location(game_t* game) {
}
