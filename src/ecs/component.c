#include <assert.h>
#include <stdlib.h>

#include "mud/data/hash_table.h"
#include "mud/ecs/component.h"
#include "mud/ecs/entity.h"
#include "mud/util/muduuid.h"

/**
 * Creates a new instance of component_t
 *
 * Returns the new instance of component_t
**/
component_t* create_component_t() {
  component_t* component = calloc(1, sizeof *component);

  component->entities = create_hash_table_t();
  component->entities->deallocator = deallocate_component_data_t;

  return component;
}

/**
 * Frees an allocated instance of component_t
 *
 * component - the component to free
**/
void free_component_t(component_t* component) {
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
void deallocate_component_t(void* value) {
  assert(value);

  free_component_t(value);
}

/**
 * Creates a new instance of component_data_t
 *
 * Returns the new instance of component_data_t
**/
component_data_t* create_component_data_t() {
  component_data_t* component_data = calloc(1, sizeof *component_data);

  component_data->ref = 0;

  return component_data;
}

/**
 * Frees an instance of component_data_t
 *
 * component_data - the component_data_t to free
**/
void free_component_data_t(component_data_t* component_data) {
  assert(component_data);

  free(component_data);
}

/**
 * Deallocates a void pointer to component_data_t
 *
 * value - void pointer to component_data_t
**/
void deallocate_component_data_t(void* value) {
  assert(value);

  free_component_data_t(value);
}

/**
 * Checks if an entity has a component.
 *
 * component - component to check against
 * entity - entity to check
 *
 * Returns true if entity has component or false otherwise
**/
bool component_has_entity(component_t* component, entity_t* entity) {
  return hash_table_has(component->entities, uuid_str(&entity->id));
}
