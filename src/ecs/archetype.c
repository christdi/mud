#include <assert.h>
#include <stdlib.h>

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/archetype.h"
#include "mud/ecs/component.h"
#include "mud/ecs/entity.h"
#include "mud/log.h"
#include "mud/util/muduuid.h"

/**
 * Allocates a new insstance of archetype_t
 *
 * Returns the allocated instance
 **/
archetype_t* ecs_new_archetype_t() {
  archetype_t* archetype = calloc(1, sizeof(archetype_t));

  archetype->components = create_linked_list_t();
  archetype->entities = create_hash_table_t();

  return archetype;
}

/**
 * Frees an allocated instance of archetype_t
 **/
void ecs_free_archetype_t(archetype_t* archetype) {
  assert(archetype);

  free_linked_list_t(archetype->components);
  free_hash_table_t(archetype->entities);

  free(archetype);
}

/**
 * Frees a void pointer to an instance of archetype_t
 *
 * value - void pointer to instance of archetype_t
 **/
void ecs_deallocate_archetype_t(void* value) {
  assert(value);

  ecs_free_archetype_t(value);
}

/**
 * Adds a component to the archetype
 *
 * archetype - archetype to add to
 * component - component to be added
 **/
void ecs_add_archetype_component(archetype_t* archetype, component_t* component) {
  list_add(archetype->components, component);
}

/**
 * Removes a component from the archetype
 *
 * archetype - arched to remove from
 * component - component to be removed
 **/
void ecs_remove_archetype_component(archetype_t* archetype, component_t* component) {
  list_remove(archetype->components, component);
}

/**
 * Checks if a given entity matches an archetype by checking if it appears
 * in all components the archetype wraps.
 *
 * archetype - the archetype to check against
 * entity - the entity to check
 *
 * Returns true if match or false otherwise
 **/
bool ecs_entity_matches_archetype(archetype_t* archetype, entity_t* entity) {
  it_t it = list_begin(archetype->components);

  component_t* component = NULL;

  while ((component = it_get(it)) != NULL) {
    if (!ecs_component_has_entity(component, entity)) {
      return false;
    }

    it = it_next(it);
  }

  return true;
}

/**
 * Adds an entity to this archetype indicating said entity matches this archetype.
 *
 * archetype - the archetype to add the entity to
 * entity - the entity to add
 **/
void ecs_add_entity_to_archetype(archetype_t* archetype, entity_t* entity) {
  hash_table_insert(archetype->entities, uuid_str(&entity->id), entity);
}

/**
 * Removes an entity from this archetype indciating said entity no longer matches this archetype.
 *
 * archetype - the archetype to remove the entity from
 * entity - the entity to remove
 **/
void ecs_remove_entity_from_archetype(archetype_t* archetype, entity_t* entity) {
  hash_table_delete(archetype->entities, uuid_str(&entity->id));
}

/**
 * Checks if an entity belongs to a particular archetype.
 *
 * archetype - the archetype to check
 * entity - the entity to check for
 **/
bool ecs_archetype_has_entity(archetype_t* archetype, entity_t* entity) {
  return hash_table_has(archetype->entities, uuid_str(&entity->id));
}

/**
 * Updates an entity against all registered archetypes
 *
 * archetypes - list of all registered archetypes
 * entity - the entity to be updated
 **/
void ecs_update_entity_archetypes(linked_list_t* archetypes, entity_t* entity) {
  it_t it = list_begin(archetypes);
  archetype_t* archetype = NULL;

  while ((archetype = it_get(it)) != NULL) {
    if (ecs_entity_matches_archetype(archetype, entity)) {
      if (!ecs_archetype_has_entity(archetype, entity)) {
        ecs_add_entity_to_archetype(archetype, entity);
      }
    } else {
      if (ecs_archetype_has_entity(archetype, entity)) {
        ecs_remove_entity_from_archetype(archetype, entity);
      }
    }

    it = it_next(it);
  }
}
