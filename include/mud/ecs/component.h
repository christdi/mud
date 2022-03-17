#ifndef MUD_ECS_COMPONENT_H
#define MUD_ECS_COMPONENT_H

#include <stdbool.h>

/**
 * Forward declrations
**/
typedef struct entity entity_t;
typedef struct hash_table hash_table_t;

/**
 * Structs
**/
typedef struct component {
  hash_table_t* entities;
} component_t;

typedef struct component_data {
  entity_t* entity;
  int ref;
} component_data_t;

/**
 * Functions
**/
component_t* ecs_create_component_t();
void ecs_free_component_t(component_t* component);
void ecs_deallocate_component_t(void* value);

component_data_t* ecs_create_component_data_t();
void ecs_free_component_data_t(component_data_t* component_data);
void ecs_deallocate_component_data_t(void* value);

void ecs_add_entity_to_component(component_t* component, component_data_t* data, linked_list_t* archetypes, entity_t* entity);
void ecs_remove_entity_from_component(component_t* component, linked_list_t* archetypes, entity_t* entity);
bool ecs_component_has_entity(component_t* component, entity_t* entity);

#endif