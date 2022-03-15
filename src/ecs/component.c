#include <assert.h>
#include <stdlib.h>

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/archetype.h"
#include "mud/ecs/component.h"
#include "mud/ecs/entity.h"
#include "mud/util/muduuid.h"

/**
 * Creates a new instance of component_t
 *
 * Returns the new instance of component_t
**/
component_t* ecs_create_component_t() {
  component_t* component = calloc(1, sizeof *component);

  component->entities = create_hash_table_t();
  component->entities->deallocator = ecs_deallocate_component_data_t;

  return component;
}

/**
 * Frees an allocated instance of component_t
 *
 * component - the component to free
**/
void ecs_free_component_t(component_t* component) {
  assert(component);

  if (component->entities != NULL) {
    free_hash_table_t(component->entities);
  }

  free(component);
}

/**
 * Deallocates a void pointer to a component_t
 *
 * value - void pointer to component_t
**/
void ecs_deallocate_component_t(void* value) {
  assert(value);

  ecs_free_component_t(value);
}

/**
 * Creates a new instance of component_data_t
 *
 * Returns the new instance of component_data_t
**/
component_data_t* ecs_create_component_data_t() {
  component_data_t* component_data = calloc(1, sizeof *component_data);

  component_data->ref = 0;

  return component_data;
}

/**
 * Frees an instance of component_data_t
 *
 * component_data - the component_data_t to free
**/
void ecs_free_component_data_t(component_data_t* component_data) {
  assert(component_data);

  free(component_data);
}

/**
 * Deallocates a void pointer to component_data_t
 *
 * value - void pointer to component_data_t
**/
void ecs_deallocate_component_data_t(void* value) {
  assert(value);

  ecs_free_component_data_t(value);
}

/**
 * Adds an entity to a component and updates the archetypes the entity belongs to.
 *
 * component - the component to add the entity to
 * archetypes - archetypes to assess against the entity
 * entity - the entity to be added to the component
**/
void ecs_add_entity_to_component(component_t* component, component_data_t* data, linked_list_t* archetypes, entity_t* entity) {
  assert(component);
  assert(data);
  assert(archetypes);
  assert(entity);

  if (ecs_component_has_entity(component, entity)) {
    return;
  }

  hash_table_insert(component->entities, uuid_str(&entity->id), data);
  ecs_update_entity_archetypes(archetypes, entity);
}

/**
 * Removes an entity from a component and updates the archetypes the entity belongs to.
 *
 * component - the component to remove the entity from
 * archetypes - archetypes to assess against the entity
 * entity - the entity to be removed from the component
**/
void ecs_remove_entity_from_component(component_t* component, linked_list_t* archetypes, entity_t* entity) {
  assert(component);
  assert(archetypes);
  assert(entity);

  if (!ecs_component_has_entity(component, entity)) {
    return;
  }

  hash_table_delete(component->entities, uuid_str(&entity->id));
  ecs_update_entity_archetypes(archetypes, entity);
}

/**
 * Checks if an entity has a component.
 *
 * component - component to check against
 * entity - entity to check
 *
 * Returns true if entity has component or false otherwise
**/
bool ecs_component_has_entity(component_t* component, entity_t* entity) {
  assert(component);
  assert(entity);

  return hash_table_has(component->entities, uuid_str(&entity->id));
}
