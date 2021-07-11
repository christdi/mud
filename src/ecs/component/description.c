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

  return description;
}

/**
 * Free a description_t.
**/
void free_description_t(description_t* description) {
  if (description->name) {
    free(description->name);
    description->name = NULL;
  }

  if (description->description) {
    free(description->description);
    description->description = NULL;
  }

  free(description);
  description = NULL;
}

/**
 * Determines if a given uuid character details registered.
 *
 * Returns 1 if true or 0 if false.
**/
int has_description(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  return hash_table_has(components->description, entity->uuid);
}

/**
 * Register a character detail components for a given uuid with active game components.
**/
void register_description(components_t* components, description_t* description) {
  assert(components);
  assert(description);

  if (hash_table_insert(components->description, description->uuid, description) != 0) {
    zlog_error(gc, "Failed to register description component for entity uuid [%s]", description->uuid);
  }
}

/**
 * Removes character details component for a given uuid.
 *
 * Returns the removed description_t in case it has to be freed, or null if
 * there was no matching uuid.
**/
description_t* unregister_description(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  return (description_t*)hash_table_delete(components->description, entity->uuid);
}

/**
 * Retrieves character details component for a given uuid.
 *
 * Returns the description_t or null if not found.
**/
description_t* get_description(components_t* components, entity_t* entity) {
  assert(components);
  assert(entity);

  return (description_t*)hash_table_get(components->description, entity->uuid);
}

/**
 * Updates registered character details.  This is currently no-op.
**/
void update_description(game_t* game) {
}
