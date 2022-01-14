#include <assert.h>
#include <stdlib.h>

#include "mud/data/hash_table.h"
#include "mud/ecs/component.h"

component_t* create_component_t() {
  component_t* component = calloc(1, sizeof *component);

  component->entities = create_hash_table_t();
  component->entities->deallocator = deallocate_component_data_t;

  return component;
}

void free_component_t(component_t* component) {
  assert(component);

  if (component->entities != NULL) {
    free_hash_table_t(component->entities);
  }

  free(component);
}

void deallocate_component_t(void* value) {
  assert(value);

  free_component_t(value);
}

component_data_t* create_component_data_t() {
  component_data_t* component_data = calloc(1, sizeof *component_data);

  component_data->ref = 0;

  return component_data;
}

void free_component_data_t(component_data_t* component_data) {
  assert(component_data);

  free(component_data);
}

void deallocate_component_data_t(void* value) {
  assert(value);

  free_component_data_t(value);
}
