#include "mud/ecs/component/description.h"
#include "mud/data/hash_table.h"
#include "mud/ecs/components.h"
#include "mud/log.h"

#include <assert.h>
#include <stdlib.h>

/**
 * Allocate memory for and initialise a new description_t.
 *
 * Returns the newly allocated description_t
**/
description_t* create_description_t() {
  description_t* description = calloc(1, sizeof *description);

  description->name = NULL;
  description->description = NULL;

  return description;
}

/**
 * Free a description_t.
**/
void free_description_t(description_t* description) {
  if (description->name != NULL) {
    free(description->name);
  }

  if (description->description != NULL) {
    free(description->description);
  }

  free(description);
}

/**
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
**/
void deallocate_description_t(void* value) {
  assert(value);

  description_t* description = (description_t*)value;

  free_description_t(description);
}

/**
 * Determines if a given uuid character details registered.
 *
 * Returns 1 if true or 0 if false.
**/
int has_description(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  return hash_table_has(components->description, entity->id.uuid);
}

/**
 * Register a character detail components for a given uuid with active game components.
**/
void register_description(components_t* components, description_t* description) {
  assert(components);
  assert(description);

  if (hash_table_insert(components->description, description->entity_id.uuid, description) != 0) {
    mlog(ERROR, "register_description", "Failed to register description component for entity uuid [%s]", description->entity_id.uuid);
  }
}

/**
 * Removes character details component for a given uuid.
**/
void unregister_description(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  hash_table_delete(components->description, entity->id.uuid);
}

/**
 * Retrieves character details component for a given uuid.
 *
 * Returns the description_t or null if not found.
**/
description_t* get_description(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  return (description_t*)hash_table_get(components->description, entity->id.uuid);
}

/**
 * Updates registered character details.  This is currently no-op.
**/
void update_description(game_t* game) {
}
